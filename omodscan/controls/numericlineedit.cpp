#include <float.h>
#include <QKeyEvent>
#include <QIntValidator>
#include "ansiutils.h"
#include "qhexvalidator.h"
#include "quintvalidator.h"
#include "qint64validator.h"
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
    setInputMode(Int32Mode);
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
/// \brief NumericLineEdit::setInputMode
/// \param on
///
void NumericLineEdit::setInputMode(InputMode mode)
{
    if(!_minValue.isValid() || !_maxValue.isValid() || mode != _inputMode)
    {
        switch(mode)
        {
            case HexMode:
            case AnsiMode:
                _minValue = (ushort)0;
                _maxValue = USHRT_MAX;
            break;

            case Int32Mode:            
                _minValue = INT_MIN;
                _maxValue = INT_MAX;
            break;

            case UInt32Mode:
                _minValue = 0U;
                _maxValue = UINT_MAX;
            break;

            case FloatMode:
                _minValue = -FLT_MAX;
                _maxValue = FLT_MAX;
            break;

            case DoubleMode:
                _minValue = -DBL_MAX;
                _maxValue = DBL_MAX;
            break;

            case Int64Mode:
                _minValue = QVariant::fromValue(INT64_MIN);
                _maxValue = QVariant::fromValue(INT64_MAX);
            break;

            case UInt64Mode:
                _minValue = 0;
                _maxValue = QVariant::fromValue(UINT64_MAX);
            break;
        }
    }
    _inputMode = mode;
    emit rangeChanged(_minValue, _maxValue);
}

///
/// \brief NumericLineEdit::codepage
///
QString NumericLineEdit::codepage() const
{
    return _codepage;
}

///
/// \brief NumericLineEdit::setCodepage
/// \param name
///
void NumericLineEdit::setCodepage(const QString& name)
{
    _codepage = name;
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
        case Int32Mode:
            value = qBound(_minValue.toInt(), value.toInt(), _maxValue.toInt());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toInt(), _paddingZeroWidth, 10, QLatin1Char('0'));
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else
            {
                const auto text = QString::number(value.toInt());
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
        break;

        case UInt32Mode:
            value = qBound(_minValue.toUInt(), value.toUInt(), _maxValue.toUInt());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toUInt(), _paddingZeroWidth, 10, QLatin1Char('0'));
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else
            {
                const auto text = QString::number(value.toUInt());
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
        break;

        case HexMode:
        {
            value = qBound(_minValue.toInt() > 0 ? _minValue.toUInt() : 0, value.toUInt(), _maxValue.toUInt());
            const QString prefix = (hasFocus() ? "" : "0x");
            if(_paddingZeroes)
            {
                const auto text = prefix + QStringLiteral("%1").arg(value.toUInt(), _paddingZeroWidth, 16, QLatin1Char('0')).toUpper();
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else
            {
                const auto text = prefix + QString("%1").arg(value.toUInt(), -1, 16).toUpper();
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
        }
        break;

        case AnsiMode:
        {
            value = qBound(_minValue.toInt() > 0 ? _minValue.toUInt() : 0, value.toUInt(), _maxValue.toUInt());
            const auto text = printableAnsi(uint16ToAnsi(value.value<quint16>()), _codepage);
            if(text != QLineEdit::text())
                QLineEdit::setText(text);
        }
        break;

        case FloatMode:
        {
            value = qBound(_minValue.toFloat(), value.toFloat(), _maxValue.toFloat());
            const auto text = QLocale().toString(value.toFloat());
            if(text != QLineEdit::text())
                QLineEdit::setText(text);
        }
        break;

        case DoubleMode:
        {
            value = qBound(_minValue.toDouble(), value.toDouble(), _maxValue.toDouble());
            const auto text = QLocale().toString(value.toFloat());
            if(text != QLineEdit::text())
                QLineEdit::setText(text);
        }
        break;

        case Int64Mode:
        {
            value = qBound(_minValue.toLongLong(), value.toLongLong(), _maxValue.toLongLong());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toLongLong(), _paddingZeroWidth, 10, QLatin1Char('0'));
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else
            {
                const auto text = QString::number(value.toLongLong());
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
        }
        break;

        case UInt64Mode:
        {
            value = qBound(_minValue.toULongLong(), value.toULongLong(), _maxValue.toULongLong());
            if(_paddingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toULongLong(), _paddingZeroWidth, 10, QLatin1Char('0'));
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else
            {
                const auto text = QString::number(value.toULongLong());
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
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
/// \brief NumericLineEdit::updateValue
///
void NumericLineEdit::updateValue()
{
    switch(_inputMode)
    {
        case Int32Mode:
        {
            bool ok;
            const auto value = text().toInt(&ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;

        case UInt32Mode:
        {
            bool ok;
            const auto value = text().toUInt(&ok);
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

        case AnsiMode:
        {
            auto codec = QTextCodec::codecForName(_codepage.toUtf8());
            if(codec == nullptr) codec = QTextCodec::codecForLocale();
            const auto value = uint16FromAnsi(codec->fromUnicode(text()));
            internalSetValue(value);
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

        case Int64Mode:
        {
            bool ok;
            const auto value = text().toLongLong(&ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;

        case UInt64Mode:
        {
            bool ok;
            const auto value = text().toULongLong(&ok);
            if(ok) internalSetValue(value);
            else internalSetValue(_value);
        }
        break;
    }
}

///
/// \brief NumericLineEdit::focusInEvent
/// \param e
///
void NumericLineEdit::focusInEvent(QFocusEvent* e)
{
    updateValue();
    QLineEdit::focusInEvent(e);
}

///
/// \brief NumberLineEdit::focusOutEvent
/// \param e
///
void NumericLineEdit::focusOutEvent(QFocusEvent* e)
{
    updateValue();
    QLineEdit::focusOutEvent(e);
}

///
/// \brief NumericLineEdit::keyPressEvent
/// \param e
///
void NumericLineEdit::keyPressEvent(QKeyEvent* e)
{
    QLineEdit::keyPressEvent(e);
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
        case Int32Mode:
        {
            bool ok;
            const auto valueInt = text.toInt(&ok);
            if(ok) value = qBound(_minValue.toInt(), valueInt, _maxValue.toInt());
        }
        break;

        case UInt32Mode:
        {
            bool ok;
            const auto valueUInt = text.toUInt(&ok);
            if(ok) value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
        }
        break;

        case HexMode:
        {
            bool ok;
            const auto valueUInt = text.toUInt(&ok, 16);
            if(ok) value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
        }
        break;

        case AnsiMode:
        {
            const uint valueUInt = uint16FromAnsi(text.toLocal8Bit());
            value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
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

        case Int64Mode:
        {
            bool ok;
            const auto valueLongLong = text.toLongLong(&ok);
            if(ok) value = qBound(_minValue.toLongLong(), valueLongLong, _maxValue.toLongLong());
        }
        break;

        case UInt64Mode:
        {
            bool ok;
            const auto valueULongLong = text.toULongLong(&ok);
            if(ok) value = qBound(_minValue.toULongLong(), valueULongLong, _maxValue.toULongLong());
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
    const bool isBlocked = signalsBlocked();
    if(!isBlocked) blockSignals(true);

    setValidator(nullptr);
    switch(_inputMode)
    {
        case Int32Mode:
        {
            const int nums = QString::number(top.toInt()).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(2, nums + 1));
            setValidator(new QIntValidator(bottom.toInt(), top.toInt(), this));

        }
        break;

        case UInt32Mode:
        {
            const int nums = QString::number(top.toUInt()).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(1, nums));
            setValidator(new QUIntValidator(bottom.toUInt(), top.toUInt(), this));
        }
        break;

        case HexMode:
        {
            const int nums = QString::number(top.toUInt(), 16).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(1, nums + 2));
            setValidator(new QHexValidator(bottom.toUInt(), top.toUInt(), this));
        }
        break;

        case AnsiMode:
            setMaxLength(2);
        break;

        case FloatMode:
        case DoubleMode:
            setMaxLength(INT16_MAX);
            setValidator(new QDoubleValidator(bottom.toDouble(), top.toDouble(), 6, this));
        break;

        case Int64Mode:
        {
            const int nums = QString::number(top.toLongLong()).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(2, nums + 1));
            setValidator(new QInt64Validator(bottom.toLongLong(), top.toLongLong(), this));
        }
        break;

        case UInt64Mode:
        {
            const int nums = QString::number(top.toULongLong()).length();
            _paddingZeroWidth = qMax(1, nums);
            setMaxLength(qMax(1, nums));
            setValidator(new QUIntValidator(bottom.toULongLong(), top.toULongLong(), this));
        }
        break;
    }
    internalSetValue(_value);
    if(!isBlocked) blockSignals(false);
}
