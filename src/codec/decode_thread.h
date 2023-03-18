#pragma once
#include "base_thread.h"
#include "decode.h"
class CODEC_API DecodeThread :
    public BaseThread
{
public:
    bool Open(AVCodecParameters* para);
    
    AVFrame* GetFrame();
private:
    void Do(AVPacket* pkt) override;
    void Main() override;


private:
    std::mutex mux_;
    Decode decode_;
    CAVPacketList pkt_list_;
    AVFrame* frame_ = nullptr;//解码后存储
    bool need_view_ = false; //每帧渲染一次
};

