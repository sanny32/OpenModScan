#include "numericcombobox.h"

///
/// \brief NumericComboBox::NumericComboBox
/// \param parent
///
NumericComboBox::NumericComboBox(QWidget* parent)
    :QComboBox(parent)
{
}

///
/// \brief NumericComboBox::addValue
/// \param value
///
void NumericComboBox::addValue(int value)
{
    addItem(QString::number(value));
}

///
/// \brief NumericComboBox::currentValue
/// \return
///
int NumericComboBox::currentValue() const
{
    return currentText().toInt();
}

///
/// \brief NumericComboBox::setCurrentValue
/// \param value
///
void NumericComboBox::setCurrentValue(int value)
{
    const auto idx = findText(QString::number(value));
    setCurrentIndex(idx);
}
