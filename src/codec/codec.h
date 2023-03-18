#pragma once
#include <mutex>
#include"base_thread.h"



class CODEC_API Codec
{
public:
	static AVCodecContext* Create(int code_id, bool is_encode);

	//���c������Ϊnullptr�����avcodec_free_context
	void set_c(AVCodecContext* c);

	//�ڴ�������ǰ����
	bool SetOpt(const char* key, const char* val);
	bool SetOpt(const char* key, int val);

	bool Open();
	//���ɹ���Ҫ���ⲿ����av_frame_free��ʧ���򷵻�NULLPTR
	AVFrame* CreateFrame();
protected:
	AVCodecContext* c_ = nullptr;
	std::mutex mux_;
	
};

