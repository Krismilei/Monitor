#include "mux_thread.h"
#include"base_thread.h"
#include<mutex>
#include<iostream>
using namespace std;
extern "C"
{ 
#include <libavformat/avformat.h>
}

bool MuxThread::Open(const char* url, AVCodecParameters* video_para, AVRational* video_time_base, AVCodecParameters* audio_para, AVRational* audio_time_base)
{
	AVFormatContext* c = xmux_.Open(url, video_para, audio_para);
    c->streams[xmux_.video_index()]->time_base = *video_time_base;
    if (xmux_.audio_index() >= 1)
    {
        c->streams[xmux_.audio_index()]->time_base = *audio_time_base;
    }
	xmux_.set_c(c);
	return true;
}

bool MuxThread::Open(const char* url, std::shared_ptr<ParaTb> vpara, std::shared_ptr<ParaTb> apara)
{
    AVCodecParameters* video_para = nullptr;
    AVCodecParameters* audio_para = nullptr;
    AVRational* video_time_base = nullptr;
    AVRational* audio_time_base = nullptr;
    if (vpara)
    {
        video_para = vpara->para;
        video_time_base = vpara->time_base;
    }
    if (apara)
    {
        audio_para = apara->para;
        audio_time_base = apara->time_base;
    }
    return Open(url, video_para, video_time_base, audio_para, audio_time_base);
}

void MuxThread::Do(AVPacket* pkt)
{
    pkts_.Push(pkt);
    Next(pkt);
}

void MuxThread::Main()
{
    xmux_.WriteHead();
    //找到关键帧
    while (!is_exit_)
    {
        unique_lock<mutex> lock(mux_);
        auto pkt = pkts_.Pop();
        if (!pkt)
        {
            this_thread::sleep_for(1ms);
            continue;
        }
        if (pkt->stream_index == xmux_.video_index()
            && pkt->flags & AV_PKT_FLAG_KEY)
        {
            xmux_.Write(pkt);
            av_packet_free(&pkt);
            break;
        }
        //丢掉非视频关键帧
        av_packet_free(&pkt);
    }
    while (!is_exit_)
    {
        //unique_lock<mutex> lock(mux_);
        AVPacket* pkt = pkts_.Pop();
        if (!pkt)
        {
            this_thread::sleep_for(1ms);
            continue;
        }

        xmux_.Write(pkt);
        av_packet_free(&pkt);
        cout << "W";
        this_thread::sleep_for(1ms);
    }
    xmux_.WriteEnd();
    xmux_.set_c(nullptr);
}
