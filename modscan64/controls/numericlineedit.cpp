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
    ,_enablePaddingZero(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);
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
    setValidator(nullptr);
    setValidator(new QIntValidator(bottom, top, this));
    _paddingZeroWidth = qMax(1, QString::number(top).length() - 1);
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
        setText(text);
    }
    else
    {
        setText(QString::number(value));
    }
}

///
/// \brief NumberLineEdit::focusOutEvent
/// \param event
///
void NumericLineEdit::focusOutEvent(QFocusEvent* event)
{
    bool ok;
    auto value = text().toInt(&ok);
    if(ok) setValue(value);
    else setValue(_value);
    QLineEdit::focusOutEvent(event);
}

///
/// \brief NumberLineEdit::on_textChanged
/// \param s
///
void NumericLineEdit::on_textChanged(const QString& s)
{
    bool ok;
    auto value = s.toInt(&ok);
    if(ok) _value = value;
}
