#pragma once

#include <QWidget>
#include "ui_xplayvideo.h"
#include "demux_thread.h"
#include "decode_thread.h"
#include "sdl.h"
class XPlayVideo : public QWidget
{
    Q_OBJECT

public:
    XPlayVideo(QWidget *parent = Q_NULLPTR);
    ~XPlayVideo();
    bool Open(const char* url);
    
    void timerEvent(QTimerEvent* ev) override;
    void Close();
    void closeEvent(QCloseEvent* ev) override;
private:
    Ui::XPlayVideo ui;
    DemuxThread demux_;
    DecodeThread decode_;
    SDL* view_ = nullptr;
};
