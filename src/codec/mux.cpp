extern "C" {
#include <libavformat/avformat.h>
}
#include "mux.h"
#include <iostream>
#include <thread>
using namespace std;

AVFormatContext* Mux::Open(const char* url, AVCodecParameters* video_para, AVCodecParameters* audio_para)
{
    AVFormatContext* c = nullptr;
    int re = avformat_alloc_output_context2(&c, nullptr, nullptr, url);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return nullptr;
    }
    if (audio_para)
    {
        AVStream* vs = avformat_new_stream(c, nullptr);
        avcodec_parameters_copy(vs->codecpar, video_para);
    }
    if (audio_para)
    {
        AVStream* as = avformat_new_stream(c, nullptr);
        avcodec_parameters_copy(as->codecpar, audio_para);
    }
    
    re = avio_open(&c->pb, url, AVIO_FLAG_WRITE);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return nullptr;
    }
    //打印输入封装信息
    av_dump_format(c, 0, url, 1);
    return c;
}



bool Mux::WriteHead()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    int re = avformat_write_header(c_, nullptr);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    //打印输出上下文
    av_dump_format(c_, 0, c_->url, 1);
    begin_audio_pts_ = -1;
    begin_video_pts_ = -1;
    return true;
}

bool Mux::Write(AVPacket* pkt)
{
    if (!pkt)return false;
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    
    //没读取到PTS，重构考虑通过duration 计算
    if (pkt->pts == AV_NOPTS_VALUE)
    {
        pkt->pts = 0;
        pkt->dts = 0;
    }

    if (pkt->stream_index == video_index_)
    {
        if (begin_video_pts_ < 0)
        {
            begin_video_pts_ = pkt->pts;
        }
        lock.unlock();//RescaleTime里已经有锁了
        RescaleTime(pkt, begin_video_pts_, video_time_base_);
        lock.lock();
    } 
    else if (pkt->stream_index == audio_index_)
    {
        if (begin_audio_pts_ < 0)
        {
            begin_audio_pts_ = pkt->pts;
        }
        lock.unlock();
        RescaleTime(pkt, begin_audio_pts_, audio_time_base_);
        lock.lock();
    }
    cout << pkt->pts << " ";
    //写入一帧数据，内部缓存排序dts
    //该函数对AVPacket的处理：
    //如果是获取数据引用，使用完引用计数减一，调用此函数后不可再访问pkt
    //如果是非引用计数，则复制，传递NULL写入interleaving queues缓冲,复制后的AVPacket会自动释放
    int re = av_interleaved_write_frame(c_, pkt);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    return true;
}

bool Mux::WriteEnd()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    int re = av_interleaved_write_frame(c_, nullptr);//写入缓冲
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    re = av_write_trailer(c_);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    return true;
}

Mux::~Mux()
{
    unique_lock<mutex> lock(mux_);
    if (audio_time_base_)
    {
        delete audio_time_base_;
        audio_time_base_ = nullptr;
    }
    if (video_time_base_)
    {
        delete video_time_base_;
        video_time_base_ = nullptr;
    }
}
