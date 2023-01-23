#include "clickablelabel.h"

///
/// \brief ClickableLabel::ClickableLabel
/// \param parent
/// \param f
///
ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
}

///
/// \brief ClickableLabel::mouseReleaseEvent
/// \param event
///
void ClickableLabel::mouseReleaseEvent(QMouseEvent* event)
{
    emit clicked();
    QLabel::mouseReleaseEvent(event);
}
