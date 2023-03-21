#include "demux_thread.h"
extern "C"
{
#include<libavformat/avformat.h>
}
#include<thread>
#include<iostream>
using namespace std;

bool DemuxThread::Open(std::string url, int timeout_ms)
{
	LOGDEBUG("DemuxTask::Open!");
	demux_.set_c(nullptr);//断开之前的连接
	this->url_ = url;
	this->timeout_ms_ = timeout_ms;
	AVFormatContext* c = demux_.Open(url.c_str());
	if (!c)return false;
	demux_.set_c(c);
	demux_.set_time_out_ms(timeout_ms);
	return true;
}

void DemuxThread::Main()
{
	AVPacket* pkt = av_packet_alloc();
	while (!is_exit_)
	{
		if (!demux_.Read(pkt))
		{
			//读取失败
			cout << "-";
			if (!demux_.is_connected())
			{
				Open(url_, timeout_ms_);
			}
			this_thread::sleep_for(10ms);
			continue;
		}
		//读取成功打印|
		//cout << "|";
		Next(pkt);
		//this_thread::sleep_for(1ms);
		//注意这里要引用计数减一！
		av_packet_unref(pkt);
		this_thread::sleep_for(1ms);
	}
}
