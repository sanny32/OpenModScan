#include "booleancombobox.h"

///
/// \brief BooleanComboBox::BooleanComboBox
/// \param parent
///
BooleanComboBox::BooleanComboBox(QWidget* parent)
    :QComboBox(parent)
{
    addItem("Disable", false);
    addItem("Enable", true);
}

///
/// \brief BooleanComboBox::currentValue
/// \return
///
bool BooleanComboBox::currentValue() const
{
    return currentData().value<bool>();
}

///
/// \brief BooleanComboBox::setCurrentValue
/// \param value
///
void BooleanComboBox::setCurrentValue(bool value)
{
    const auto idx = findData(value);
    setCurrentIndex(idx);
}
