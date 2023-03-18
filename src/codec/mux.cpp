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
    //��ӡ�����װ��Ϣ
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
    //��ӡ���������
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
    
    //û��ȡ��PTS���ع�����ͨ��duration ����
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
        lock.unlock();//RescaleTime���Ѿ�������
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
    //д��һ֡���ݣ��ڲ���������dts
    //�ú�����AVPacket�Ĵ���
    //����ǻ�ȡ�������ã�ʹ�������ü�����һ�����ô˺����󲻿��ٷ���pkt
    //����Ƿ����ü��������ƣ�����NULLд��interleaving queues����,���ƺ��AVPacket���Զ��ͷ�
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
    int re = av_interleaved_write_frame(c_, nullptr);//д�뻺��
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
