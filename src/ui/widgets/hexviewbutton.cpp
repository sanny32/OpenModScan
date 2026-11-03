#include <QIcon>
#include <QProxyStyle>
#include "hexviewbutton.h"

///
/// \brief The HexButtonStyle class
///
class HexButtonStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int pixelMetric(PixelMetric m, const QStyleOption* opt = nullptr, const QWidget* w = nullptr) const override
    {
        if(m == PM_ButtonShiftHorizontal || m == PM_ButtonShiftVertical)
            return 0;
        return QProxyStyle::pixelMetric(m, opt, w);
    }
};

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
    setStyle(new HexButtonStyle(style()));
}
