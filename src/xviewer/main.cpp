#include "xviewer.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QDir>
#include "xcamera_config.h"
#define TEST_CAM_PATH "test.db"
#include "xcamera_record.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XViewer w;
    w.show();
    return a.exec();
}
