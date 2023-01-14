#include <QPainter>
#include "customframe.h"

CustomFrame::CustomFrame(QWidget* parent, Qt::WindowFlags f)
    :QFrame(parent, f)
{

}

void CustomFrame::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidthF(0.5);

    auto oldPen = painter.pen();
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));
    painter.setPen(oldPen);

    QFrame::paintEvent(e);
}
