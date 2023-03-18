#include "encode.h"
#include <iostream>

using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}


AVPacket* Encode::Enco(const AVFrame* frame)
{
	if (!frame)return nullptr;
	unique_lock<mutex>lock(mux_);
	if (!c_)return nullptr;
	int re = avcodec_send_frame(c_, frame);
	if (re != 0)
	{
		cerr << "avcodec_send_frame failed!" << endl;
		return nullptr;
	}
	AVPacket* pkt = av_packet_alloc();
	re = avcodec_receive_packet(c_, pkt);
	if (re == 0)
	{
		return pkt;
	}
	av_packet_free(&pkt);
	if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
	{
		return nullptr;
	}
	if (re < 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << "avcodec_receive_packet failed!" << buf << endl;
	}
	return nullptr;
}

vector<AVPacket*> Encode::End()
{
	unique_lock<mutex>lock(mux_);
	if (!c_) { return vector<AVPacket*>(); }
	vector<AVPacket*> res;
	int re = avcodec_send_frame(c_, nullptr);
	if (re != 0)return res;
	while (re >= 0)
	{
		AVPacket* pkt = av_packet_alloc();
		re = avcodec_receive_packet(c_, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			break;
		}
		res.push_back(pkt);
	}
	return res;
}
