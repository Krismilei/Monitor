#pragma once
#include "format.h"
class CODEC_API Demux :
    public Format
{
public:
    static AVFormatContext* Open(const char* url);

    bool Read(AVPacket* pkt);

    bool Seek(long long pts, int stream_index);
};

