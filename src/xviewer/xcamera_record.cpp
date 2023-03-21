#include "xcamera_record.h"
#include "demux_thread.h"
#include "mux_thread.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include<iostream>
using namespace std;
using namespace chrono;

static std::string GetFileName(string path)
{
    stringstream ss;
    auto t = system_clock::to_time_t(system_clock::now());
    auto time_str = put_time(localtime(&t), "%Y_%m_%d_%H_%M_%S");
    ss << path << "/" << "cam_"<<time_str<<".mp4";
    return ss.str();
}

void XCameraRecord::Main()
{
    DemuxThread demux;
    MuxThread mux;
    if (rtsp_url_.empty())
    {
        LOGERROR("open rtsp url failed!");
        return;
    }

    //�Զ�����
    while (!is_exit_)
    {
        if (demux.Open(rtsp_url_)) 
        {
            break;
        }
        MSleep(3000);
        continue;
    }

    auto vpara = demux.CopyVideoPara();
    if (!vpara)
    {
        LOGERROR("demux.CopyVideoPara failed!");
        //��Ҫ����demux ��Դ�ͷŵ�����
        demux.Stop();
        return;
    }
    demux.Start();

    auto apara = demux.CopyAudioPara();

    AVCodecParameters* para = nullptr;  //��Ƶ����
    AVRational* timebase = nullptr;     //��Ƶʱ�����
    if (apara)
    {
        para = apara->para;
        timebase = apara->time_base;
    }

    if (!mux.Open(GetFileName(save_path_).c_str(),
        vpara->para, vpara->time_base,
        para, timebase))            
    {
        LOGERROR("mux.Open rtsp_url_ failed!");
        demux.Stop();
        mux.Stop();
        return;
    }
    demux.set_next(&mux);
    mux.Start();

    auto cur = NowMs();

    while (!is_exit_)
    {
        if (NowMs() - cur > file_sec_ * 1000)
        {
            cur = NowMs();
            mux.Stop(); //ֹͣ�洢��д������
            if (!mux.Open(GetFileName(save_path_).c_str(),
                vpara->para, vpara->time_base,
                para, timebase))            
            {
                LOGERROR("mux.Open rtsp_url_ failed!");
                demux.Stop();
                mux.Stop();
                return;
            }
            mux.Start();
        }

        MSleep(10);
    }
    
    mux.Stop();
    demux.Stop();
}