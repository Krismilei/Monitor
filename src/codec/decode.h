#pragma once
#include"codec.h"
#include<vector>
#include"base_thread.h"

class CODEC_API Decode :
    public Codec
{
public:
    //pkt传入后会复制一份(引用计数),调用完可清理avpkt
    bool Send(const AVPacket* pkt);
    //暂不支持硬解
    bool Recv(AVFrame* frame);

    std::vector<AVFrame*> End();
};

