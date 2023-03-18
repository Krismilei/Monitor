#pragma once
#include "base_thread.h"
#include"demux.h"

enum SYN_TYPE
{
    SYN_NONE = 0,  //不做同步
    SYN_VIDEO = 1, //根据视频同步，不处理音频
};

class CODEC_API DemuxThread :
    public BaseThread
{
public:
    bool Open(std::string url, int timeout_ms = 1000);

    std::shared_ptr<ParaTb> CopyVideoPara() { return demux_.CopyVideoPara(); }
    std::shared_ptr<ParaTb> CopyAudioPara() { return demux_.CopyAudioPara(); }
    void set_syn_type(SYN_TYPE t) { syn_type_ = t; }
private:
    void Main() override;

    Demux demux_;
    std::string url_;
    int timeout_ms_;
    SYN_TYPE syn_type_ = SYN_NONE;
};

