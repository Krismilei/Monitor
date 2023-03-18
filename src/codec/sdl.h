#pragma once
#include <mutex>
#include"base_thread.h"

class CODEC_API SDL
{
public:
    enum forma  //枚举的值和ffmpeg中一致
    {
        YUV420P = 0,
        /* 暂不支持
        ARGB = 25,
        RGBA = 26,
        BGRA = 28
        */
    };

	void Close();

    bool Init(int w, int h, void* win_id = nullptr, forma fmt = YUV420P);
    bool Init(AVCodecParameters* para);

    bool Draw(
        const unsigned  char* y, int y_pitch,
        const unsigned  char* u, int u_pitch,
        const unsigned  char* v, int v_pitch
    );

    bool DrawFrame(AVFrame* frame);

    void set_win_id(void* win_id);

    

private:
    bool InitVideo();

private:
    SDL_Window* win_ = nullptr;
    SDL_Renderer* render_ = nullptr;
    SDL_Texture* texture_ = nullptr;
    std::mutex mux_;
    bool is_first_InitVideo_ = true;
    int width_ = 0;
    int height_ = 0;
    forma fmt_ = YUV420P;
    void* win_id_ = nullptr; //窗口句柄
};

