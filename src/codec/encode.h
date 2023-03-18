#pragma once

#include"codec.h"
#include<vector>

class CODEC_API Encode :
    public Codec
{
public:
    /// �������� �̰߳�ȫ ÿ���´���AVPacket
    /// @para frame �ռ����û�ά��
    /// @return ʧ�ܷ�Χnullptr ���ص�AVPacket�û���Ҫͨ��av_packet_free ����
    AVPacket* Enco(const AVFrame* frame);

    std::vector<AVPacket*> End();
};

