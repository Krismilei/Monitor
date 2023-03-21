#include "xcalendar.h"
#include <QPainter>
XCalendar::XCalendar(QWidget* p):QCalendarWidget(p)
{
}

void XCalendar::paintCell(QPainter* painter,
    const QRect& rec,
    const QDate& date) const
{
    //有视频的日期特殊显示
    if (mdate_.find(date) == mdate_.end()) 
    {
        QCalendarWidget::paintCell(painter, rec, date);
        return;
    }

    

    auto font = painter->font();

    font.setPixelSize(40);

    if (date == selectedDate())
    {
        painter->setBrush(QColor(118, 178, 224));
        painter->drawRect(rec);                 
    }
    painter->setFont(font);             
    painter->setPen(QColor(255, 0, 0)); 
    painter->drawText(rec,Qt::AlignCenter,
        QString::number(date.day()));

}