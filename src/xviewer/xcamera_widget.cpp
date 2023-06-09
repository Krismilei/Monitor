#include "xcamera_widget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>
#include <QListWidget>
#include "demux_thread.h"
#include "decode_thread.h"
#include "sdl.h"
#include "xcamera_config.h"

void XCameraWidget::paintEvent(QPaintEvent* p)
{
    //渲染样式表
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
XCameraWidget::XCameraWidget(QWidget* p):QWidget(p)
{
    //接收拖拽
    this->setAcceptDrops(true);
}

//拖拽进入
void XCameraWidget::dragEnterEvent(QDragEnterEvent* e)
{
    //接收拖拽进入
    e->acceptProposedAction();
}

void XCameraWidget::Draw()
{
    if (!demux_ || !decode_ || !view_)return;
    auto f = decode_->GetFrame();
    if (!f)return;
    view_->DrawFrame(f);
    FreeFrame(&f);
}

XCameraWidget::~XCameraWidget()
{
    if (demux_)
    {
        demux_->Stop();
        delete demux_;
        demux_ == nullptr;
    }
    if (decode_)
    {
        decode_->Stop();
        delete decode_;
        decode_ == nullptr;
    }
    if (view_)
    {
        view_->Close();
        delete view_;
        view_ = nullptr;
    }
}

bool XCameraWidget::Open(const char* url)
{
    if (demux_)
        demux_->Stop();
    if (decode_)
        decode_->Stop();
    demux_ = new DemuxThread();
    if (!demux_->Open(url))
    {
        return false;
    }
    decode_ = new DecodeThread();
    auto para = demux_->CopyVideoPara();
    if (!decode_->Open(para->para))
    {
        return false;
    }

    demux_->set_next(decode_);

    view_ = new SDL();
    view_->set_win_id((void*)winId());
    view_->Init(para->para);

    demux_->Start();
    decode_->Start();
    return true;
}
//拖拽松开
void XCameraWidget::dropEvent(QDropEvent* e)
{
    //拿到url
    qDebug()<<e->source()->objectName();
    auto wid = (QListWidget*)e->source();
    qDebug() << wid->currentRow();
    auto cam = XCameraConfig::Instance()->GetCam(wid->currentRow());
    Open(cam.sub_url);

}