#include "codec.h"
#include <iostream>

using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
//预处理指令导入库
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

AVCodecContext* Codec::Create(int code_id, bool is_encode)
{
	AVCodec* codec = nullptr;
	if (is_encode)
	{
		codec = avcodec_find_encoder((AVCodecID)code_id);
	} 
	else
	{
		codec = avcodec_find_decoder((AVCodecID)code_id);
	}
	if (!codec)
	{
		cerr << "avcodec_find_encoder||avcodec_find_decoder failed!" << endl;
		return nullptr;
	}

	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return nullptr;
	}
	//设置默认值
	c->time_base = { 1,25 };
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->thread_count = 16;
	//在外部还需设置宽和高等
	return c;
}

void Codec::set_c(AVCodecContext* c)
{
	unique_lock<mutex>lock(mux_);
	//需要先清理上下文
	if (c_)
	{
		avcodec_free_context(&c);
	}
	c_ = c;
	return;
}

bool Codec::SetOpt(const char* key, const char* val)
{
	if (!key || !val || !c_)
	{
		return false;
	}
	unique_lock<mutex>lock(mux_);
	int re = av_opt_set(c_->priv_data, key, val, 0);
	if (re != 0) 
	{
		cerr << "SetOpt failed!" << endl;
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << buf << endl;
		return false;
	}
	return true;
}

bool Codec::SetOpt(const char* key, int val)
{
	if (!key||!c_)
	{
		return false;
	}
	unique_lock<mutex>lock(mux_);
	int re = av_opt_set_int(c_->priv_data, key, val, 0);
	if (re != 0)
	{
		cerr << "SetOpt failed!" << endl;
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << buf << endl;
		return false;
	}
	return true;
}

bool Codec::Open()
{
	unique_lock<mutex>lock(mux_);
	if (!c_) {
		return false;
	}
	int re = avcodec_open2(c_, nullptr, nullptr);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf - 1));
		cerr << "avcodec_open2 failed!" << buf << endl;
		return false;
	}
	return true;
}

AVFrame* Codec::CreateFrame()
{
	AVFrame* frame = av_frame_alloc();
	if (frame == nullptr)
	{
		cerr << "av_frame_alloc failed!" << endl;
		return nullptr;
	}
	frame->width = c_->width;
	frame->height = c_->height;
	frame->format = c_->pix_fmt;
	int re = av_frame_get_buffer(frame, 0);
	if (re != 0) {
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << "av_frame_get_buffer failed!" << buf << endl;
		av_frame_free(&frame);
		return nullptr;
	}
	return frame;
}
