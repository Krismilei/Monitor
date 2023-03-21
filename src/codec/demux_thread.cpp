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
	demux_.set_c(nullptr);//�Ͽ�֮ǰ������
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
			//��ȡʧ��
			cout << "-";
			if (!demux_.is_connected())
			{
				Open(url_, timeout_ms_);
			}
			this_thread::sleep_for(10ms);
			continue;
		}
		//��ȡ�ɹ���ӡ|
		//cout << "|";
		Next(pkt);
		//this_thread::sleep_for(1ms);
		//ע������Ҫ���ü�����һ��
		av_packet_unref(pkt);
		this_thread::sleep_for(1ms);
	}
}
