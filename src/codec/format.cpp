#include "format.h"
#include <iostream>
#include <thread>
#include "base_thread.h"
using namespace std;
extern "C" { 
#include <libavformat/avformat.h>
}
//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
using namespace std;

static int TimeoutCallback(void* para)
{
	auto xf = (Format*)para;
	if (xf->IsTimeOut())return 1;//超时退出Read
	return 0;//正常阻塞
}

bool Format::CopyPara(int stream_index, AVCodecParameters* dst)
{
	unique_lock<mutex> lock(mux_);
	if (!c_)
	{
		return false;
	}
	if (stream_index<0 || stream_index>c_->nb_streams)
	{
		return false;
	}
	int re = avcodec_parameters_copy(dst, c_->streams[stream_index]->codecpar);
	if (re < 0)
	{
		return false;
	}
	return true;
}

bool Format::CopyPara(int stream_index, AVCodecContext* dst)
{
	unique_lock<mutex> lock(mux_);
	if (!c_)return false;
	int re = avcodec_parameters_to_context(dst, c_->streams[stream_index]->codecpar);
	if (re < 0)
	{
		return false;
	}
	return true;
}

std::shared_ptr<ParaTb> Format::CopyVideoPara()
{
	unique_lock<mutex> lock(mux_);
	if (!c_ || video_index() < 0)return nullptr;
	std::shared_ptr<ParaTb> re(ParaTb::Create());
	*re->time_base = c_->streams[video_index()]->time_base;
	avcodec_parameters_copy(re->para, c_->streams[video_index()]->codecpar);
	return re;
}

std::shared_ptr<ParaTb> Format::CopyAudioPara()
{
	unique_lock<mutex> lock(mux_);
	if (!c_ || audio_index() < 0)return nullptr;
	std::shared_ptr<ParaTb> re(ParaTb::Create());
	*re->time_base = c_->streams[audio_index()]->time_base;
	avcodec_parameters_copy(re->para, c_->streams[audio_index()]->codecpar);
	return re;
}

void Format::set_c(AVFormatContext* c)
{
	unique_lock<mutex> lock(mux_);
	//先清理
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
	if (c_)
	{
		if (c_->oformat)
		{
			if (c_->pb)
			{
				avio_closep(&c_->pb);
			}
			avformat_free_context(c_);
		}
		else if (c_->iformat)
		{
			avformat_close_input(&c_);
		}
		else
		{
			avformat_free_context(c_);
		}
	}
	c_ = c;
	if (!c_)
	{
		is_connected_ = false;
		return;
	}
	is_connected_ = true;
	last_time_ = NowMs();
	if (time_out_ms_ > 0)
	{
		AVIOInterruptCB cb = { TimeoutCallback,this };
		c_->interrupt_callback = cb;
	}

	//-1表示没有音频流或视频流
	audio_index_ = -1;
	video_index_ = -1;
	for (int i = 0; i < c->nb_streams; i++) {
		if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audio_index_ = i;
			audio_time_base_ = new AVRational;
			audio_time_base_->den = c->streams[i]->time_base.den;
			audio_time_base_->num = c->streams[i]->time_base.num;
		} else if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_index_ = i;
			video_time_base_ = new AVRational;
			video_time_base_->den = c->streams[i]->time_base.den;
			video_time_base_->num = c->streams[i]->time_base.num;
			video_codec_id_ = c->streams[i]->codecpar->codec_id;
		}
	}
}

bool Format::RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* origin_time_base)
{
	unique_lock<mutex> lock(mux_);
	if (!c_)return false;
	pkt->pts = av_rescale_q_rnd(pkt->pts - offset_pts, *origin_time_base,
		c_->streams[pkt->stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->dts = av_rescale_q_rnd(pkt->dts - offset_pts, *origin_time_base,
		c_->streams[pkt->stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->duration = av_rescale_q(pkt->duration, *origin_time_base, c_->streams[pkt->stream_index]->time_base);
	pkt->pos = -1;
	return true;
}

void Format::set_time_out_ms(int ms)
{
	unique_lock<mutex> lock(mux_);
	this->time_out_ms_ = ms;
	//设置回调函数，处理超时退出
	if (c_)
	{
		AVIOInterruptCB cb = { TimeoutCallback,this };
		c_->interrupt_callback = cb;
	}
}

inline bool Format::IsTimeOut()
{
	if (NowMs() - last_time_ > time_out_ms_)
	{
		last_time_ = NowMs();
		is_connected_ = false;
		return true;
	} else {
		return false;
	}
}
