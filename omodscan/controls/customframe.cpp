#include <QPainter>
#include "customframe.h"

///
/// \brief CustomFrame::CustomFrame
/// \param parent
/// \param f
///
CustomFrame::CustomFrame(QWidget* parent, Qt::WindowFlags f)
    :QFrame(parent, f)
{
}

///
/// \brief CustomFrame::paintEvent
/// \param e
///
void CustomFrame::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    QPen pen;
    pen.setColor(palette().dark().color());
    pen.setWidthF(0.5);

    auto oldPen = painter.pen();
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));
    painter.setPen(oldPen);

    QFrame::paintEvent(e);
}
