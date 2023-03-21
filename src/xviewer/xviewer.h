#pragma once

#include <QtWidgets/QWidget>
#include "ui_xviewer.h"
#include <QMenu>
class XViewer : public QWidget
{
    Q_OBJECT

public:
    XViewer(QWidget *parent = Q_NULLPTR);

    //鼠标事件 用于拖动窗口
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;

    //窗口大小发生编码
    void resizeEvent(QResizeEvent* ev) override;
    //右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override;

    //预览视频窗口
    void View(int count);

    //刷新左侧相机列表
    void RefreshCams();

    //编辑摄像机
    void SetCam(int index);

    //定时器渲染视频
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
