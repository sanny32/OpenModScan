#include <QPalette>
#include "tscrollarea.h"

///
/// \brief TScrollArea::TScrollArea
/// \param parent
///
TScrollArea::TScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::transparent);
    setPalette(p);
}
