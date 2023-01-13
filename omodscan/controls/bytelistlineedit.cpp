#include <QRegularExpressionValidator>
#include "bytelistlineedit.h"

///
/// \brief ByteListLineEdit::ByteListLineEdit
/// \param parent
///
ByteListLineEdit::ByteListLineEdit(QWidget* parent)
    :QLineEdit(parent)
{
    setInputMode(DecMode);

    connect(this, &QLineEdit::editingFinished, this, &ByteListLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &ByteListLineEdit::on_textChanged);
}

///
/// \brief ByteListLineEdit::ByteListLineEdit
/// \param mode
/// \param parent
///
ByteListLineEdit::ByteListLineEdit(InputMode mode, QWidget *parent)
    :QLineEdit(parent)
{
    setInputMode(mode);

    connect(this, &QLineEdit::editingFinished, this, &ByteListLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &ByteListLineEdit::on_textChanged);
}

///
/// \brief ByteListLineEdit::value
/// \return
///
QByteArray ByteListLineEdit::value() const
{
    return _value;
}

///
/// \brief ByteListLineEdit::setValue
/// \param value
///
void ByteListLineEdit::setValue(const QByteArray& value)
{
    switch(_inputMode)
    {
        case DecMode:
        {
            QStringList text;
            for(auto&& v : value)
                text.push_back(QString::number((quint8)v));
            QLineEdit::setText(text.join(','));
            QLineEdit::setCursorPosition(0);
        }
        break;

        case HexMode:
        {
            QStringList text;
            for(auto&& v : value)
                text.push_back(QString("%1").arg((quint8)v, 2, 16, QLatin1Char('0')));
            QLineEdit::setText(text.join(',').toUpper());
            QLineEdit::setCursorPosition(0);
        }
        break;
    }

    if(value != _value)
    {
        _value = value;
        emit valueChanged(_value);
    }
}

///
/// \brief ByteListLineEdit::inputMode
/// \return
///
ByteListLineEdit::InputMode ByteListLineEdit::inputMode() const
{
    return _inputMode;
}

///
/// \brief ByteListLineEdit::setInputMode
/// \param mode
///
void ByteListLineEdit::setInputMode(InputMode mode)
{
    _inputMode = mode;

    setValidator(nullptr);
    switch(mode)
    {
        case DecMode:
            setValidator(new QRegularExpressionValidator(QRegularExpression("(?:[0-9]{1,2}[,]{0,1})*"), this));
        break;

        case HexMode:
            setValidator(new QRegularExpressionValidator(QRegularExpression("(?:[0-9a-fA-F]{1,2}[,]{0,1})*"), this));
        break;
    }
    setValue(_value);
}

///
/// \brief ByteListLineEdit::setText
/// \param text
///
void ByteListLineEdit::setText(const QString& text)
{
    QLineEdit::setText(text);
    updateValue();
}

///
/// \brief ByteListLineEdit::focusOutEvent
/// \param event
///
void ByteListLineEdit::focusOutEvent(QFocusEvent* event)
{
    updateValue();
    QLineEdit::focusOutEvent(event);
}

///
/// \brief ByteListLineEdit::on_editingFinished
///
void ByteListLineEdit::on_editingFinished()
{
    updateValue();
}

///
/// \brief ByteListLineEdit::on_textChanged
/// \param text
///
void ByteListLineEdit::on_textChanged(const QString& text)
{
    QByteArray value;
    switch(_inputMode)
    {
        case DecMode:
        {
            for(auto&& s : text.split(','))
            {
                bool ok;
                const quint8 v = s.toUInt(&ok);
                if(ok) value.push_back(v);
            }
        }
        break;

        case HexMode:
        {
            for(auto&& s : text.split(','))
            {
                bool ok;
                const quint8 v = s.toUInt(&ok, 16);
                if(ok) value.push_back(v);
            }
        }
        break;
    }

    if(!value.isEmpty() && value != _value)
    {
        _value = value;
        emit valueChanged(_value);
    }
}

///
/// \brief ByteListLineEdit::updateValue
///
void ByteListLineEdit::updateValue()
{
    QByteArray value;
    switch(_inputMode)
    {
        case DecMode:
        {
            for(auto&& s : text().split(','))
            {
                bool ok;
                const quint8 v = s.toUInt(&ok);
                if(ok) value.push_back(v);
            }

            if(!value.isEmpty()) setValue(value);
            else setValue(_value);
        }
        break;

        case HexMode:
        {
            for(auto&& s : text().split(','))
            {
                bool ok;
                const quint8 v = s.toUInt(&ok, 16);
                if(ok) value.push_back(v);
            }

            if(!value.isEmpty()) setValue(value);
            else setValue(_value);
        }
        break;
    }
}
