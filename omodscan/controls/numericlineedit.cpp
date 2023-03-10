#include <QDebug>
#include <float.h>
#include <QIntValidator>
#include "qhexvalidator.h"
#include "numericlineedit.h"

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param parent
///
NumericLineEdit::NumericLineEdit(QWidget* parent)
    : QLineEdit(parent)
    ,_paddingZeroes(false)
    ,_paddingZeroWidth(0)
{
    setInputMode(DecMode);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &NumericLineEdit::on_textChanged);
    connect(this, &NumericLineEdit::rangeChanged, this, &NumericLineEdit::on_rangeChanged);
}

///
/// \brief NumericLineEdit::NumericLineEdit
/// \param mode
/// \param parent
///
NumericLineEdit::NumericLineEdit(NumericLineEdit::InputMode mode, QWidget *parent)
    : QLineEdit(parent)
    ,_paddingZeroes(false)
    ,_paddingZeroWidth(0)
{
    setInputMode(mode);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &NumericLineEdit::on_textChanged);
    connect(this, &NumericLineEdit::rangeChanged, this, &NumericLineEdit::on_rangeChanged);
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
/// \brief NumericLineEdit::inputMode
/// \return
///
NumericLineEdit::InputMode NumericLineEdit::inputMode() const
{
    return _inputMode;
}

///
/// \brief NumericLineEdit::setHexInput
/// \param on
///
void NumericLineEdit::setInputMode(InputMode mode)
{
    _inputMode = mode;
    if(!_minValue.isValid() || !_maxValue.isValid())
    {
        switch(mode)
        {
            case DecMode:
            case HexMode:
                _minValue = INT_MIN;
                _maxValue = INT_MAX;
            break;

            case FloatMode:
                _minValue = -FLT_MAX;
                _maxValue = FLT_MAX;
            break;

            case DoubleMode:
                _minValue = -DBL_MAX;
                _maxValue = DBL_MAX;
            break;
        }
    }
    emit rangeChanged(_minValue, _maxValue);
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
/// \brief NumericLineEdit::internalSetValue
/// \param value
///
void NumericLineEdit::internalSetValue(QVariant value)
{
    switch(_inputMode)
    {
        case DecMode:
            value = qBound(_minValue.toInt(), value.toInt(), _maxValue.toInt());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toInt(), _paddingZeroWidth, 10, QLatin1Char('0'));
                QLineEdit::setText(text);
            }
            else
            {
                const auto text = QString::number(value.toInt());
                QLineEdit::setText(text);
            }
        break;

        case HexMode:
            value = qBound(_minValue.toInt() > 0 ? _minValue.toUInt() : 0, value.toUInt(), _maxValue.toUInt());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toUInt(), _paddingZeroWidth, 16, QLatin1Char('0'));
                QLineEdit::setText(text.toUpper());
            }
            else
            {
                const auto text = QString("%1").arg(value.toUInt(), -1, 16);
                QLineEdit::setText(text.toUpper());
            }
        break;

        case FloatMode:
            value = qBound(_minValue.toFloat(), value.toFloat(), _maxValue.toFloat());
            QLineEdit::setText(QLocale().toString(value.toFloat()));
        break;

        case DoubleMode:
            value = qBound(_minValue.toDouble(), value.toDouble(), _maxValue.toDouble());
            QLineEdit::setText(QLocale().toString(value.toDouble()));
        break;
    }

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
    switch(_inputMode)
    {
        case DecMode:
        {
            bool ok;
            const auto value = text().toInt(&ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;

        case HexMode:
        {
            bool ok;
            const auto value = text().toUInt(&ok, 16);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;

        case FloatMode:
        {
            bool ok;
            const auto value = QLocale().toFloat(text(), &ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;

        case DoubleMode:
        {
            bool ok;
            const auto value = QLocale().toDouble(text(), &ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;
    }
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
void NumericLineEdit::on_textChanged(const QString& text)
{
    QVariant value;
    switch(_inputMode)
    {
        case DecMode:
        {
            bool ok;
            const auto valueInt = text.toInt(&ok);
            if(ok) value = qBound(_minValue.toInt(), valueInt, _maxValue.toInt());
        }
        break;

        case HexMode:
        {
            bool ok;
            const auto valueUInt = text.toUInt(&ok, 16);
            if(ok) value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
        }
        break;

        case FloatMode:
        {
            bool ok;
            const auto valueFloat = QLocale().toFloat(text, &ok);
            if(ok) value = qBound(_minValue.toFloat(), valueFloat, _maxValue.toFloat());
        }
        break;

        case DoubleMode:
        {
            bool ok;
            const auto valueDouble = QLocale().toDouble(text, &ok);
            if(ok) value = qBound(_minValue.toDouble(), valueDouble, _maxValue.toDouble());
        }
        break;
    }

    if(value.isValid() && value != _value)
    {
        _value = value;
        emit valueChanged(_value);
    }
}

///
/// \brief NumericLineEdit::on_rangeChanged
/// \param bottom
/// \param top
///
void NumericLineEdit::on_rangeChanged(const QVariant& bottom, const QVariant& top)
{
    blockSignals(true);
    setValidator(nullptr);
    switch(_inputMode)
    {
        case DecMode:
        {
            const int nums = QString::number(top.toInt()).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(1, nums));
            setValidator(new QIntValidator(bottom.toInt(), top.toInt(), this));

        }
        break;

        case HexMode:
        {
            const int nums = QString::number(top.toUInt(), 16).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(1, nums));
            setValidator(new QHexValidator(bottom.toUInt(), top.toUInt(), this));
        }
        break;

        case FloatMode:
        case DoubleMode:
            setMaxLength(INT16_MAX);
            setValidator(new QDoubleValidator(bottom.toDouble(), top.toDouble(), 6, this));
        break;
    }
    internalSetValue(_value);
    blockSignals(false);
}
