#include <QPalette>
#include "transpscrollarea.h"

///
/// \brief TranspScrollArea::TranspScrollArea
/// \param parent
///
TranspScrollArea::TranspScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::transparent);
    setPalette(p);
}
