#include <QDebug>
#include <QIntValidator>
#include "qhexvalidator.h"
#include "numericlineedit.h"

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param parent
///
NumericLineEdit::NumericLineEdit(QWidget* parent)
    : QLineEdit(parent)
    ,_value(0)
    ,_paddingZeroWidth(0)
    ,_paddingZeroes(false)
    ,_hexInput(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &NumericLineEdit::on_textChanged);
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
    ,_paddingZeroes(false)
    ,_hexInput(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &NumericLineEdit::on_textChanged);
}

///
/// \brief NumericLineEdit::paddingZeroes
/// \return
///
bool NumericLineEdit::paddingZeroes() const
{
    return _paddingZeroes;
}

///
/// \brief NumberLineEdit::enablePaddingZero
/// \param on
///
void NumericLineEdit::setPaddingZeroes(bool on)
{
    _paddingZeroes = on;
}

///
/// \brief NumericLineEdit::range
/// \return
///
QRange<int> NumericLineEdit::range() const
{
    const auto validator = (QIntValidator*)this->validator();
    return QRange<int>(validator->bottom(), validator->top());
}

///
/// \brief NumericLineEdit::setInputRange
/// \param range
///
void NumericLineEdit::setInputRange(QRange<int> range)
{
    setInputRange(range.from(), range.to());
}

///
/// \brief NumberLineEdit::setInputRange
/// \param bottom
/// \param top
///
void NumericLineEdit::setInputRange(int bottom, int top)
{
    const int nums = QString::number(top, _hexInput ? 16 : 10).length();
    _paddingZeroWidth = qMax(1, nums);
    setMaxLength(qMax(1, nums));

    setValidator(nullptr);
    setValidator(_hexInput ? new QHexValidator(bottom, top, this) :
                             new QIntValidator(bottom, top, this));
}

///
/// \brief NumericLineEdit::hexInput
/// \return
///
bool NumericLineEdit::hexInput() const
{
    return _hexInput;
}

///
/// \brief NumericLineEdit::setHexInput
/// \param on
///
void NumericLineEdit::setHexInput(bool on)
{
    _hexInput = on;
    setInputRange(range());
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
    internalSetValue(value);
    if(_paddingZeroes)
    {
        const auto text = QStringLiteral("%1").arg(_value, _paddingZeroWidth, _hexInput ? 16 : 10, QLatin1Char('0'));
        QLineEdit::setText(text.toUpper());
    }
    else
    {
        const auto text = QString::number(_value, _hexInput ? 16 : 10);
        QLineEdit::setText(text.toUpper());
    }
}

///
/// \brief NumericLineEdit::internalSetValue
/// \param value
///
void NumericLineEdit::internalSetValue(int value)
{
    const auto validator = (QIntValidator*)this->validator();
    if(value < validator->bottom()) value = validator->bottom();
    if(value > validator->top()) value = validator->top();

    if(value != _value)
    {
        _value = value;
        emit valueChanged(_value);
    }
}

///
/// \brief NumericLineEdit::updateValue
///
void NumericLineEdit::updateValue()
{
    bool ok;
    const auto value = text().toInt(&ok, _hexInput ? 16 : 10);
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

///
/// \brief NumericLineEdit::on_textChanged
///
void NumericLineEdit::on_textChanged(const QString&)
{
    bool ok;
    const auto value = text().toInt(&ok, _hexInput ? 16 : 10);
    if(ok) internalSetValue(value);
}
