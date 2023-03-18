#pragma once
#include <mutex>
#include"base_thread.h"



class CODEC_API Codec
{
public:
	static AVCodecContext* Create(int code_id, bool is_encode);

	//如果c——不为nullptr会调用avcodec_free_context
	void set_c(AVCodecContext* c);

	//在打开上下文前设置
	bool SetOpt(const char* key, const char* val);
	bool SetOpt(const char* key, int val);

	bool Open();
	//若成功需要在外部调用av_frame_free，失败则返回NULLPTR
	AVFrame* CreateFrame();
protected:
	AVCodecContext* c_ = nullptr;
	std::mutex mux_;
	
};

