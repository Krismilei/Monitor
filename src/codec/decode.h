#pragma once
#include"codec.h"
#include<vector>
#include"base_thread.h"

class CODEC_API Decode :
    public Codec
{
public:
    //pkt�����Ḵ��һ��(���ü���),�����������avpkt
    bool Send(const AVPacket* pkt);
    //�ݲ�֧��Ӳ��
    bool Recv(AVFrame* frame);

    std::vector<AVFrame*> End();
};

