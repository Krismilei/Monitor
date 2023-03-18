#include "decode_thread.h"
#include <iostream>
#include "base_thread.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

}
using namespace std;

bool DecodeThread::Open(AVCodecParameters* para)
{
    if (!para)
    {
        LOGERROR("para is NULLPTR");
        return false;
    }
    unique_lock<mutex> lock(mux_);
    AVCodecContext* c = decode_.Create(para->codec_id, 0);
    avcodec_parameters_to_context(c, para);
    decode_.set_c(c);
    if (!decode_.Open())
    {
        LOGERROR("decode_.Open() failed! ");
        return false;
    }
    LOGINFO("decode_.Open() SUCCESS!");
    return true;
}

void DecodeThread::Do(AVPacket* pkt)
{
    cout << "#";
    if (!pkt || pkt->stream_index != 0)//暂时仅处理视频
    {
        return;
    }
    pkt_list_.Push(pkt);
}

void DecodeThread::Main()
{
    {
        unique_lock<mutex> lock(mux_);
        if (!frame_) { frame_ = av_frame_alloc(); }
    }

    while (!is_exit_)
    {
        auto pkt = pkt_list_.Pop();
        if (!pkt)
        {
            this_thread::sleep_for(1ms);
            continue;
        }
        //发送到解码线程
        bool re = decode_.Send(pkt);
        //pkt传入后会复制一份(引用计数),调用完可清理avpkt
        av_packet_free(&pkt);
        if (!re)
        {
            this_thread::sleep_for(1ms);
            continue;
        }

        {
            unique_lock<mutex> lock(mux_);
            if (decode_.Recv(frame_))
            {
                need_view_ = true;
            }
            cout << "@";
        }
        
        //sleep不能放在锁里
        this_thread::sleep_for(1ms);
    }

    {
        unique_lock<mutex> lock(mux_);
        if (frame_) { av_frame_free(&frame_); }
    }
    
    
}

AVFrame* DecodeThread::GetFrame()
{
    unique_lock<mutex> lock(mux_);
    if (!need_view_ || !frame_||!frame_->buf[0])return nullptr;
    AVFrame* f = av_frame_alloc();
    int re = av_frame_ref(f, frame_);
    if (re != 0)
    {
        av_frame_free(&f);
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf - 1));
        cerr << buf << endl;
        return nullptr;
    }
    need_view_ = false;
    return f;
}
