#include "formattedspinbox.h"

///
/// \brief FormattedSpinBox::FormattedSpinBox
/// \param parent
///
FormattedSpinBox::FormattedSpinBox(QWidget* parent)
    :QSpinBox(parent)
{
}

///
/// \brief FormattedSpinBox::textFromValue
/// \param val
/// \return
///
QString FormattedSpinBox::textFromValue(int val) const
{
    const int nums = qMax(1, QString::number(maximum()).length());
    const bool hexValue = (displayIntegerBase() == 16);
    return hexValue ? "0x" + QStringLiteral("%1").arg(val, 0, 16).toUpper():
        QStringLiteral("%1").arg(val, nums, displayIntegerBase(), QLatin1Char('0'));
}
