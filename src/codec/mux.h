#pragma once
#include "format.h"
class CODEC_API Mux :
    public Format
{
public:
    static AVFormatContext* Open(const char* url, AVCodecParameters* video_para = nullptr, AVCodecParameters* audio_para = nullptr);
    bool WriteHead();
    bool Write(AVPacket* pkt);
    bool WriteEnd();
    ~Mux();

private:
    long long begin_video_pts_ = -1;//原视频开始时间
    long long begin_audio_pts_ = -1;//原音频开始时间
};

