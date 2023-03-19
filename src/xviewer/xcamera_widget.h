#pragma once
#include <QWidget>
class DecodeThread;
class SDL;
class DemuxThread;
class XCameraWidget :public QWidget
{
    Q_OBJECT

public:
    XCameraWidget(QWidget* p=nullptr);
    
    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* e) override;

    //拖拽松开
    void dropEvent(QDropEvent* e) override;

    //渲染
    void paintEvent(QPaintEvent* p);

    //打开rtsp 开始解封装 解码
    bool Open(const char* url);


    //渲染视频
    void Draw();

    //清理资源
    ~XCameraWidget();
private:
    DecodeThread* decode_ = nullptr;
    DemuxThread* demux_ = nullptr;
    SDL* view_ = nullptr;
};

