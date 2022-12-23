#include <QIntValidator>
#include "numberlineedit.h"

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param parent
///
NumberLineEdit::NumberLineEdit(QWidget* parent)
    : QLineEdit(parent)
    ,_value(0)
    ,_paddingZeroWidth(0)
    ,_allowLeadingZeroFormatter(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);

    connect(this, &QLineEdit::textChanged, this, &NumberLineEdit::on_textChanged);
}

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param s
/// \param parent
///
NumberLineEdit::NumberLineEdit(const QString& s, QWidget* parent)
    : QLineEdit(s, parent)
    ,_value(0)
    ,_paddingZeroWidth(0)
    ,_allowLeadingZeroFormatter(false)
{
    setInputRange(INT_MIN, INT_MAX);
    setValue(0);
}

///
/// \brief NumberLineEdit::allowPaddingZeroFormatter
/// \param on
///
void NumberLineEdit::allowPaddingZeroFormatter(bool on)
{
    _allowLeadingZeroFormatter = on;
}

///
/// \brief NumberLineEdit::setInputRange
/// \param bottom
/// \param top
///
void NumberLineEdit::setInputRange(int bottom, int top)
{
    setValidator(nullptr);
    setValidator(new QIntValidator(bottom, top, this));
    _paddingZeroWidth = qMax(1, QString::number(top).length() - 1);
}

///
/// \brief NumberLineEdit::value
/// \return
///
int NumberLineEdit::value()
{
    return _value;
}

///
/// \brief NumberLineEdit::setValue
/// \param value
///
void NumberLineEdit::setValue(int value)
{
    _value = value;
    const auto validator = (QIntValidator*)this->validator();

    if(value < validator->bottom()) value = validator->bottom();
    if(value > validator->top()) value = validator->top();
    if(_allowLeadingZeroFormatter)
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
void NumberLineEdit::focusOutEvent(QFocusEvent* event)
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
void NumberLineEdit::on_textChanged(const QString& s)
{
    bool ok;
    auto value = s.toInt(&ok);
    if(ok) _value = value;
}
