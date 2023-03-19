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
    
    //��ק����
    void dragEnterEvent(QDragEnterEvent* e) override;

    //��ק�ɿ�
    void dropEvent(QDropEvent* e) override;

    //��Ⱦ
    void paintEvent(QPaintEvent* p);

    //��rtsp ��ʼ���װ ����
    bool Open(const char* url);


    //��Ⱦ��Ƶ
    void Draw();

    //������Դ
    ~XCameraWidget();
private:
    DecodeThread* decode_ = nullptr;
    DemuxThread* demux_ = nullptr;
    SDL* view_ = nullptr;
};

