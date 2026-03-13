#include <QIcon>
#include "hexviewbutton.h"

///
/// \brief HexViewButton::HexViewButton
/// \param parent
///
HexViewButton::HexViewButton(QWidget* parent)
    : QToolButton(parent)
{
    setCheckable(true);
    setAutoRaise(true);
    setCursor(Qt::ArrowCursor);
    setIcon(QIcon(":/res/icon-hex.svg"));
    setToolTip(tr("Hex View"));
}
