#include"base_thread.h"
extern "C" {
#include <libavformat/avformat.h>
}
#include "demux.h"
#include <iostream>
#include <thread>
using namespace std;

AVFormatContext* Demux::Open(const char* url)
{
    AVFormatContext* c = nullptr;
    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "stimeout", "1000000", 0);//���ӳ�ʱ1��
    int re = avformat_open_input(&c, url, nullptr, &opts);
    if (opts)
    {
        av_dict_free(&opts);
    }
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re,buf,sizeof(buf)-1);
        cerr << buf << endl;
        return nullptr;
    }
    re = avformat_find_stream_info(c, nullptr);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return nullptr;
    }
    //��ӡ�����װ��Ϣ
    av_dump_format(c, 0, url, 0);
    return c;
}

bool Demux::Read(AVPacket* pkt)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    //ÿ�ε��ö������AVPacket�ڲ��ռ�,�������ͷ�
    //���pktʹ�ý�������Ҫ��av_packet_unref()�������ü���
    int re = av_read_frame(c_, pkt);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    last_time_ = NowMs();
    return true;
}

bool Demux::Seek(long long pts, int stream_index)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    int re = av_seek_frame(c_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cerr << buf << endl;
        return false;
    }
    return true;
}
