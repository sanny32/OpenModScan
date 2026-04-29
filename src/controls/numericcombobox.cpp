#include <QEvent>
#include <QLineEdit>
#include <QSignalBlocker>
#include "numericcombobox.h"

///
/// \brief NumericComboBox::NumericComboBox
/// \param parent
///
NumericComboBox::NumericComboBox(QWidget* parent)
    : QComboBox(parent)
    ,_lineEdit(new NumericLineEdit(this))
{
    setEditable(true);
    setLineEdit(_lineEdit);
    connect(this, &QComboBox::currentTextChanged, this, &NumericComboBox::onCurrentTextChanged);
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
    const int clamped = qBound(minimum(), value, maximum());
    {
        QSignalBlocker b(this);
        const auto idx = findText(QString::number(clamped));
        if (idx >= 0)
            setCurrentIndex(idx);
        else
            setCurrentText(QString::number(clamped));
    }
    emit currentValueChanged(clamped);
}

///
/// \brief NumericComboBox::minimum
/// \return
///
int NumericComboBox::minimum() const
{
    return _lineEdit->range<int>().from();
}

///
/// \brief NumericComboBox::setMinimum
/// \param min
///
void NumericComboBox::setMinimum(int min)
{
    _lineEdit->setInputRange(min, _lineEdit->range<int>().to());
}

///
/// \brief NumericComboBox::maximum
/// \return
///
int NumericComboBox::maximum() const
{
    return _lineEdit->range<int>().to();
}

///
/// \brief NumericComboBox::setMaximum
/// \param max
///
void NumericComboBox::setMaximum(int max)
{
    _lineEdit->setInputRange(_lineEdit->range<int>().from(), max);
}

///
/// \brief NumericComboBox::setRange
/// \param min
/// \param max
///
void NumericComboBox::setRange(int min, int max)
{
    _lineEdit->setInputRange(min, max);
}

///
/// \brief NumericComboBox::focusOutEvent
/// Clamps the entered value to the valid range when focus leaves the widget.
///
void NumericComboBox::focusOutEvent(QFocusEvent* e)
{
    bool ok;
    const int value = currentText().toInt(&ok);
    const int clamped = ok ? qBound(minimum(), value, maximum()) : minimum();
    setCurrentValue(clamped);
    QComboBox::focusOutEvent(e);
}

///
/// \brief NumericComboBox::onCurrentTextChanged
/// Emits currentValueChanged when the user selects or types a valid integer.
///
void NumericComboBox::onCurrentTextChanged(const QString& text)
{
    bool ok;
    const int value = text.toInt(&ok);
    if (ok)
        emit currentValueChanged(qBound(minimum(), value, maximum()));
}
