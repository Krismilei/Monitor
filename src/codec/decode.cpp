#include "decode.h"
#include <iostream>

using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

using namespace std;

bool Decode::Send(const AVPacket* pkt)
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return false;
    //pkt传入后会复制一份(引用计数),调用完可清理avpkt
    int re = avcodec_send_packet(c_, pkt);
    if (re != 0)return false;
    return true;
}

bool Decode::Recv(AVFrame* frame)
{
    unique_lock<mutex>lock(mux_);
    if (!c_)return false;
    int re = avcodec_receive_frame(c_, frame);
    if (re == 0)
    {
        return true;
    }
    return false;
}

std::vector<AVFrame*> Decode::End()
{
    unique_lock<mutex>lock(mux_);
    if (!c_){ return std::vector<AVFrame*>(); }
    vector<AVFrame*> res;
    int re = avcodec_send_packet(c_, nullptr);
    while (re >= 0)
    {
        AVFrame* frame = av_frame_alloc();
        re = avcodec_receive_frame(c_, frame);
        if (re < 0)
        {
            av_frame_free(&frame);
            return res;
        }
        res.push_back(frame);
    }
    return res;
}
