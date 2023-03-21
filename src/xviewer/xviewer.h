#pragma once

#include <QtWidgets/QWidget>
#include "ui_xviewer.h"
#include <QMenu>
class XViewer : public QWidget
{
    Q_OBJECT

public:
    XViewer(QWidget *parent = Q_NULLPTR);

    //����¼� �����϶�����
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;

    //���ڴ�С��������
    void resizeEvent(QResizeEvent* ev) override;
    //�Ҽ��˵�
    void contextMenuEvent(QContextMenuEvent* event) override;

    //Ԥ����Ƶ����
    void View(int count);

    //ˢ���������б�
    void RefreshCams();

    //�༭�����
    void SetCam(int index);

    //��ʱ����Ⱦ��Ƶ
    void timerEvent(QTimerEvent* ev) override;
public slots:
    void MaxWindow();
    void NormalWindow();
    void View1();
    void View4();
    void View9();
    void View16();
    void AddCam(); 
    void SetCam();  
    void DelCam();  

    void StartRecord(); 
    void StopRecord();  
    void Preview();
    void Playback();

    void SelectCamera(QModelIndex index);
    void SelectDate(QDate date);        
    void PlayVideo(QModelIndex index);  
private:
    Ui::XViewerClass ui;
    QMenu left_menu_;
};
