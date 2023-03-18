#pragma once
#include"base_thread.h"
#include"mux.h"

class CODEC_API MuxThread :
    public BaseThread
{
public:
    bool Open(const char* url,
        AVCodecParameters* video_para = nullptr,
        AVRational* video_time_base = nullptr,
        AVCodecParameters* audio_para = nullptr,
        AVRational* audio_time_base = nullptr
    );
    bool Open(const char* url, std::shared_ptr<ParaTb> vpara, std::shared_ptr<ParaTb>apara);
private:
    void Do(AVPacket* pkt)override;
    
    void Main()override;

private:
    Mux xmux_;
    std::mutex mux_;
    CAVPacketList pkts_;
};

