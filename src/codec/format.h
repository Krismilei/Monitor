#pragma once
#include<mutex>
#include"base_thread.h"


class CODEC_API Format
{
public:
	bool CopyPara(int stream_index, AVCodecParameters* dst);
	bool CopyPara(int stream_index, AVCodecContext* dst);

	std::shared_ptr<ParaTb> CopyVideoPara();
	std::shared_ptr<ParaTb> CopyAudioPara();

	void set_c(AVFormatContext* c);

	int video_index() { return video_index_; }
	int audio_index() { return audio_index_; }

	AVRational* video_time_base() { return video_time_base_; }
	AVRational* audio_time_base() { return audio_time_base_; }

	bool RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* origin_time_base);

	int video_codec_id() { return video_codec_id_; }

	void set_time_out_ms(int ms);
	inline bool IsTimeOut();

	bool is_connected() { return is_connected_; }

protected:
	bool is_connected_ = false;
	int time_out_ms_ = 0;
	long long last_time_ = 0;
	AVFormatContext* c_ = nullptr;
	std::mutex mux_;
	//video和audio在stream中的索引
	int video_index_ = 0;
	int audio_index_ = 1;
	AVRational* video_time_base_ = nullptr;
	AVRational* audio_time_base_ = nullptr;
	int video_codec_id_ = 0;
};

