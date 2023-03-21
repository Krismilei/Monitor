#pragma once
#include "sdl.h"
#include "sdl/SDL.h"
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avutil.lib")
using namespace std;
#pragma comment(lib,"SDL2.lib")

void SDL::Close()
{
    unique_lock<mutex> lock(mux_);
    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }
    if (win_)
    {
        SDL_DestroyWindow(win_);
        win_ = nullptr;
    }
}

bool SDL::Init(int w, int h, void* win_id, forma fmt)
{
    if (w <= 0 || h <= 0)return false;
    //��ʼ��SDL ��Ƶ��
    InitVideo();

    unique_lock<mutex> sdl_lock(mux_);
    width_ = w;
    height_ = h;
    fmt_ = fmt;
    if (win_id)win_id_ = win_id;

    if (texture_)
        SDL_DestroyTexture(texture_);
    if (render_)
        SDL_DestroyRenderer(render_);

    ///1 ��������
    if (!win_)
    {
        if (!win_id_)
        {
            //�½�����
            win_ = SDL_CreateWindow("",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
            );
        } else
        {
            //��Ⱦ���ؼ�����
            win_ = SDL_CreateWindowFrom(win_id_);
        }
    }
    if (!win_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    /// 2 ������Ⱦ��
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    if (!render_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    //�������� ���Դ棩
    texture_ = SDL_CreateTexture(render_,
        SDL_PIXELFORMAT_IYUV,           
        SDL_TEXTUREACCESS_STREAMING,    //Ƶ���޸ĵ���Ⱦ��������
        w, h                            
    );
    if (!texture_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }
    return true;
}

bool SDL::Init(AVCodecParameters* para)
{
    if (!para)return false;
    switch (para->format)
    {
    //YUVJ420P��YUV420P����ʽһ��
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
        return Init(para->width, para->height);
    default:
        return false;
    }
}

bool SDL::Draw(const unsigned char* y, int y_pitch, const unsigned char* u, int u_pitch, const unsigned char* v, int v_pitch)
{
    if (!y || !u || !v)return false;
    unique_lock<mutex> sdl_lock(mux_);
    if (!texture_ || !render_ || !win_ || width_ <= 0 || height_ <= 0)
        return false;

    auto re = SDL_UpdateYUVTexture(texture_,
        NULL,
        y, y_pitch,
        u, u_pitch,
        v, v_pitch);
    if (re != 0)
    {
        cout << SDL_GetError() << endl;
        return false;
    }
    //�����Ļ
    SDL_RenderClear(render_);

    //���ʸ��Ƶ���Ⱦ��
    SDL_Rect* prect = nullptr;
    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0)
    {
        cout << SDL_GetError() << endl;
        return false;
    }
    SDL_RenderPresent(render_);
    return true;
}

void SDL::set_win_id(void* win_id)
{
    unique_lock<mutex> sdl_lock(mux_);
    win_id_ = win_id;
}

bool SDL::DrawFrame(AVFrame* frame)
{
    if (!frame)return false;
    return Draw(frame->data[0], frame->linesize[0],//Y
        frame->data[1], frame->linesize[1],	//U
        frame->data[2], frame->linesize[2]	//V
    );
}

bool SDL::InitVideo()
{
    unique_lock<mutex> sdl_lock(mux_);
    if (!is_first_InitVideo_)return true;
    is_first_InitVideo_ = false;
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        cout << SDL_GetError() << endl;
        return false;
    }
    //�趨�����㷨������������,���Բ�ֵ�㷨
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    return true;
}
