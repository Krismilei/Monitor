#include "base_thread.h"
#include<iostream>
#include<string>

using namespace std;

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			break;
		}
	}
}

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}

void BaseThread::Start()
{
	unique_lock<mutex> lock(m_);
	static int i = 0;
	i++;
	index_ = i;
	is_exit_ = false;
	//�����߳�
	th_ = thread(&BaseThread::Main, this);
	string str = "Thread::Start() ";
	str += index_;
	LOGINFO(str.c_str());
}

void BaseThread::Stop()
{
	string str = "Thread::Stop() Begin ";
	str += index_;
	LOGINFO(str.c_str());
	is_exit_ = true;
	if (th_.joinable())//�ж����߳��Ƿ���Եȴ�
	{
		th_.join();//�ȴ����߳��˳�
	}
	str = "Thread::Stop() End ";
	str += index_;
	LOGINFO(str.c_str());
}

void BaseThread::Next(AVPacket* pkt)
{
	unique_lock<mutex> lock(m_);
	if (next_)next_->Do(pkt);
}

void BaseThread::set_next(BaseThread* xt)
{
	unique_lock<mutex> lock(m_);
	next_ = xt;
}

ParaTb::~ParaTb()
{
	if (para) { avcodec_parameters_free(&para); }
	if (time_base)
	{
		delete time_base;
		time_base = nullptr;
	}
}

ParaTb::ParaTb()
{
	para = avcodec_parameters_alloc();
	time_base = new AVRational();
}

AVPacket* CAVPacketList::Pop()
{
	unique_lock<mutex> lock(mux_);
	if (pkts_.empty())return nullptr;
	auto pkt = pkts_.front();
	pkts_.pop_front();
	return pkt;
}

void CAVPacketList::Push(AVPacket* pkt)
{
	unique_lock<mutex> lock(mux_);
	AVPacket* p = av_packet_alloc();
	av_packet_ref(p, pkt);
	pkts_.push_back(p);
	//�������ռ䣬�������ݵ��ؼ�֡λ�ã�
	//��Ƶ����ÿ֡���ǹؼ�֡��ֻ�账����Ƶ
	if (pkts_.size() > max_packets_)
	{
		//�����һ֡
		if (pkts_.front()->flags & AV_PKT_FLAG_KEY)
		{
			av_packet_free(&pkts_.front());
			pkts_.pop_front();
		}
		//�������зǹؼ�֮֡ǰ������
		while (!pkts_.empty())
		{
			if (pkts_.front()->flags & AV_PKT_FLAG_KEY)//�ؼ�֡
			{
				return;
			}
			av_packet_free(&pkts_.front());//����
			pkts_.pop_front();  //����
		}
	}
}

void FreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame))return;
	av_frame_free(frame);
}
