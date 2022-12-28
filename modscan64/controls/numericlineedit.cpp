#include <QIntValidator>
#include "numericlineedit.h"

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param parent
///
NumericLineEdit::NumericLineEdit(QWidget* parent)
    : QLineEdit(parent)
    ,_value(0)
    ,_paddingZeroWidth(0)
    ,_enablePaddingZero(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
}

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param s
/// \param parent
///
NumericLineEdit::NumericLineEdit(const QString& s, QWidget* parent)
    : QLineEdit(s, parent)
    ,_value(0)
    ,_paddingZeroWidth(0)
    ,_enablePaddingZero(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
}

///
/// \brief NumberLineEdit::enablePaddingZero
/// \param on
///
void NumericLineEdit::enablePaddingZero(bool on)
{
    _enablePaddingZero = on;
}

///
/// \brief NumberLineEdit::setInputRange
/// \param bottom
/// \param top
///
void NumericLineEdit::setInputRange(int bottom, int top)
{
    const int nums = QString::number(top).length();
    _paddingZeroWidth = qMax(1, nums - 1);

    setValidator(nullptr);
    setValidator(new QIntValidator(bottom, top, this));
    setMaxLength(qMax(1, nums));

}

///
/// \brief NumericLineEdit::setText
/// \param text
///
void NumericLineEdit::setText(const QString& text)
{
    QLineEdit::setText(text);
    updateValue();
}

///
/// \brief NumberLineEdit::value
/// \return
///
int NumericLineEdit::value()
{
    return _value;
}

///
/// \brief NumberLineEdit::setValue
/// \param value
///
void NumericLineEdit::setValue(int value)
{
    _value = value;
    const auto validator = (QIntValidator*)this->validator();

    if(value < validator->bottom()) value = validator->bottom();
    if(value > validator->top()) value = validator->top();
    if(_enablePaddingZero)
    {
        const auto text = QStringLiteral("%1").arg(value, _paddingZeroWidth, 10, QLatin1Char('0'));
        QLineEdit::setText(text);
    }
    else
    {
        QLineEdit::setText(QString::number(value));
    }
}

///
/// \brief NumericLineEdit::updateValue
///
void NumericLineEdit::updateValue()
{
    bool ok;
    const auto value = text().toInt(&ok);
    if(ok) setValue(value);
}

///
/// \brief NumberLineEdit::focusOutEvent
/// \param event
///
void NumericLineEdit::focusOutEvent(QFocusEvent* event)
{
    updateValue();
    QLineEdit::focusOutEvent(event);
}

///
/// \brief NumericLineEdit::on_editingFinished
///
void NumericLineEdit::on_editingFinished()
{
    updateValue();
}
