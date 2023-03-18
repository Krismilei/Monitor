#pragma once

#include"codec.h"
#include<vector>

class CODEC_API Encode :
    public Codec
{
public:
    /// 编码数据 线程安全 每次新创建AVPacket
    /// @para frame 空间由用户维护
    /// @return 失败范围nullptr 返回的AVPacket用户需要通过av_packet_free 清理
    AVPacket* Enco(const AVFrame* frame);

    std::vector<AVPacket*> End();
};

