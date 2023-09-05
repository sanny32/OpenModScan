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
    return  QStringLiteral("%1").arg(val, nums, 10, QLatin1Char('0'));
}
