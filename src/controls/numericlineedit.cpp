#include <float.h>
#include <QCoreApplication>
#include <QIcon>
#include <QPainter>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QIntValidator>
#include <QStyle>
#include <QStyleOptionToolButton>
#include <QToolButton>
#include "ansiutils.h"
#include "qhexvalidator.h"
#include "quintvalidator.h"
#include "qint64validator.h"
#include "numericlineedit.h"

namespace
{
class HexViewButton final : public QToolButton
{
public:
    explicit HexViewButton(QWidget* parent = nullptr)
        : QToolButton(parent)
        , _icon(":/res/icon-hex.svg")
    {
        setCheckable(true);
        setAutoRaise(true);
        setCursor(Qt::ArrowCursor);
        setToolTip(QCoreApplication::translate("HexViewButton", "Hex View"));
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.icon = QIcon();
        opt.text.clear();

        QPainter painter(this);
        style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &painter, this);

        const int maxWidth = qMax(1, opt.rect.width() - 4);
        const int maxHeight = qMax(1, opt.rect.height() - 4);
        const int defaultIconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
        const QSize requested = iconSize().isValid() ? iconSize() : QSize(defaultIconSize, defaultIconSize);
        const QSize bounded(qMin(requested.width(), maxWidth), qMin(requested.height(), maxHeight));

        QRect iconRect(QPoint(0, 0), bounded);
        iconRect.moveCenter(opt.rect.center());

        const QIcon::Mode mode = isEnabled() ? (opt.state & QStyle::State_MouseOver ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
        const QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
        _icon.paint(&painter, iconRect, Qt::AlignCenter, mode, state);
    }

private:
    QIcon _icon;
};
}

///
/// \brief NumberLineEdit::NumberLineEdit
/// \param parent
///
NumericLineEdit::NumericLineEdit(QWidget* parent)
    : QLineEdit(parent)
    ,_leadingZeroes(false)
    ,_leadingZeroWidth(0)
    ,_hexButton(new HexViewButton(this))
    ,_hexView(false)
    ,_hexButtonVisible(false)
{
    connect(_hexButton, &QToolButton::toggled, this, &NumericLineEdit::on_hexViewToggled);

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
    ,_leadingZeroes(false)
    ,_leadingZeroWidth(0)
    ,_hexButton(new HexViewButton(this))
    ,_hexView(false)
    ,_hexButtonVisible(false)
{
    connect(_hexButton, &QToolButton::toggled, this, &NumericLineEdit::on_hexViewToggled);

    setInputMode(mode);
    setValue(0);

    connect(this, &QLineEdit::editingFinished, this, &NumericLineEdit::on_editingFinished);
    connect(this, &QLineEdit::textChanged, this, &NumericLineEdit::on_textChanged);
    connect(this, &NumericLineEdit::rangeChanged, this, &NumericLineEdit::on_rangeChanged);
}

///
/// \brief NumericLineEdit::leadingZeroes
/// \return
///
bool NumericLineEdit::leadingZeroes() const
{
    return _leadingZeroes;
}

///
/// \brief NumberLineEdit::enablePaddingZero
/// \param on
///
void NumericLineEdit::setLeadingZeroes(bool on)
{
    _leadingZeroes = on;
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

    if(_hexView && !isHexViewApplicable())
    {
        _hexView = false;
        _hexButton->blockSignals(true);
        _hexButton->setChecked(false);
        _hexButton->blockSignals(false);
    }

    updateHexButton();
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
/// \brief NumericLineEdit::hexView
///
bool NumericLineEdit::hexView() const
{
    return _hexView;
}

///
/// \brief NumericLineEdit::setHexView
/// \param on
///
void NumericLineEdit::setHexView(bool on)
{
    if(!isHexViewApplicable()) return;
    _hexButton->setChecked(on); // triggers on_hexViewToggled
}

///
/// \brief NumericLineEdit::isHexViewApplicable
///
bool NumericLineEdit::isHexViewApplicable() const
{
    return _inputMode == Int32Mode  ||
           _inputMode == UInt32Mode ||
           _inputMode == Int64Mode  ||
           _inputMode == UInt64Mode;
}

///
/// \brief NumericLineEdit::updateHexButton
///
void NumericLineEdit::updateHexButton()
{
    const bool visible = _hexButtonVisible && isHexViewApplicable();
    _hexButton->setVisible(visible);

    const int btnWidth = visible ? (height() > 8 ? height() - 8 : 16) : 0;
    const int margin   = visible ? btnWidth + 6 : 0;
    setTextMargins(0, 0, margin, 0);
}

///
/// \brief NumericLineEdit::hexButtonVisible
///
bool NumericLineEdit::hexButtonVisible() const
{
    return _hexButtonVisible;
}

///
/// \brief NumericLineEdit::setHexButtonVisible
/// \param visible
///
void NumericLineEdit::setHexButtonVisible(bool visible)
{
    _hexButtonVisible = visible;
    updateHexButton();
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
            if(_hexView)
            {
                const auto uval = static_cast<uint>(value.toInt());
                const QString prefix = hasFocus() ? QString() : QStringLiteral("0x");
                const auto text = prefix + QStringLiteral("%1").arg(uval, _leadingZeroWidth, 16, QLatin1Char('0')).toUpper();
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
            if(_hexView)
            {
                const QString prefix = hasFocus() ? QString() : QStringLiteral("0x");
                const auto text = prefix + QStringLiteral("%1").arg(value.toUInt(), _leadingZeroWidth, 16, QLatin1Char('0')).toUpper();
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else if(_leadingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toUInt(), _leadingZeroWidth, 10, QLatin1Char('0'));
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
            const QString prefix = hasFocus() ? QString() : QStringLiteral("0x");
            if(_leadingZeroes)
            {
                const auto text = prefix + QStringLiteral("%1").arg(value.toUInt(), _leadingZeroWidth, 16, QLatin1Char('0')).toUpper();
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
            if(_hexView)
            {
                const auto uval = static_cast<quint64>(value.toLongLong());
                const QString prefix = hasFocus() ? QString() : QStringLiteral("0x");
                const auto text = prefix + QStringLiteral("%1").arg(uval, _leadingZeroWidth, 16, QLatin1Char('0')).toUpper();
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
            if(_hexView)
            {
                const QString prefix = hasFocus() ? QString() : QStringLiteral("0x");
                const auto text = prefix + QStringLiteral("%1").arg(value.toULongLong(), _leadingZeroWidth, 16, QLatin1Char('0')).toUpper();
                if(text != QLineEdit::text())
                    QLineEdit::setText(text);
            }
            else if(_leadingZeroes)
            {
                const auto text = QStringLiteral("%1").arg(value.toULongLong(), _leadingZeroWidth, 10, QLatin1Char('0'));
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
            if(_hexView)
            {
                bool ok;
                const auto uval = QLineEdit::text().toUInt(&ok, 16);
                if(ok) internalSetValue(static_cast<int>(uval));
                else internalSetValue(_value);
            }
            else
            {
                bool ok;
                const auto value = text().toInt(&ok);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
        }
        break;

        case UInt32Mode:
        {
            if(_hexView)
            {
                bool ok;
                const auto value = QLineEdit::text().toUInt(&ok, 16);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
            else
            {
                bool ok;
                const auto value = text().toUInt(&ok);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
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
            if(_hexView)
            {
                bool ok;
                const auto uval = QLineEdit::text().toULongLong(&ok, 16);
                if(ok) internalSetValue(static_cast<qint64>(uval));
                else internalSetValue(_value);
            }
            else
            {
                bool ok;
                const auto value = text().toLongLong(&ok);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
        }
        break;

        case UInt64Mode:
        {
            if(_hexView)
            {
                bool ok;
                const auto value = QLineEdit::text().toULongLong(&ok, 16);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
            else
            {
                bool ok;
                const auto value = text().toULongLong(&ok);
                if(ok) internalSetValue(value);
                else internalSetValue(_value);
            }
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
    internalSetValue(_value);
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
/// \brief NumericLineEdit::resizeEvent
/// \param e
///
void NumericLineEdit::resizeEvent(QResizeEvent* e)
{
    QLineEdit::resizeEvent(e);
    const int h = height() - 8;
    _hexButton->setGeometry(width() - h - 4, 4, h, h);
    _hexButton->setIconSize(QSize(h, h));
    updateHexButton();
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
            if(_hexView)
            {
                bool ok;
                const auto uval = text.toUInt(&ok, 16);
                if(ok)
                {
                    const auto ival = static_cast<int>(uval);
                    value = qBound(_minValue.toInt(), ival, _maxValue.toInt());
                }
            }
            else
            {
                bool ok;
                const auto valueInt = text.toInt(&ok);
                if(ok) value = qBound(_minValue.toInt(), valueInt, _maxValue.toInt());
            }
        }
        break;

        case UInt32Mode:
        {
            if(_hexView)
            {
                bool ok;
                const auto valueUInt = text.toUInt(&ok, 16);
                if(ok) value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
            }
            else
            {
                bool ok;
                const auto valueUInt = text.toUInt(&ok);
                if(ok) value = qBound(_minValue.toUInt(), valueUInt, _maxValue.toUInt());
            }
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
            if(_hexView)
            {
                bool ok;
                const auto uval = text.toULongLong(&ok, 16);
                if(ok)
                {
                    const auto ival = static_cast<qint64>(uval);
                    value = qBound(_minValue.toLongLong(), ival, _maxValue.toLongLong());
                }
            }
            else
            {
                bool ok;
                const auto valueLongLong = text.toLongLong(&ok);
                if(ok) value = qBound(_minValue.toLongLong(), valueLongLong, _maxValue.toLongLong());
            }
        }
        break;

        case UInt64Mode:
        {
            if(_hexView)
            {
                bool ok;
                const auto valueULongLong = text.toULongLong(&ok, 16);
                if(ok) value = qBound(_minValue.toULongLong(), valueULongLong, _maxValue.toULongLong());
            }
            else
            {
                bool ok;
                const auto valueULongLong = text.toULongLong(&ok);
                if(ok) value = qBound(_minValue.toULongLong(), valueULongLong, _maxValue.toULongLong());
            }
        }
        break;
    }

    if(value.isValid() && value != _value)
    {
        auto oldValue = _value;
        _value = value;

        emit valueChanged(_value);
        emit valueChanged(oldValue, _value);
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
            if(_hexView)
            {
                _leadingZeroWidth = qMax(1, (int)QString::number(static_cast<uint>(top.toInt()), 16).length());
                setMaxLength(_leadingZeroWidth + 2); // +2 for "0x"
                setValidator(new QHexValidator(this));
            }
            else
            {
                const int nums = QString::number(top.toInt()).length();
                setMaxLength(qMax(2, nums + 1));
                setValidator(new QIntValidator(bottom.toInt(), top.toInt(), this));
            }
        }
        break;

        case UInt32Mode:
        {
            if(_hexView)
            {
                _leadingZeroWidth = qMax(1, (int)QString::number(top.toUInt(), 16).length());
                setMaxLength(_leadingZeroWidth + 2); // +2 for "0x"
                setValidator(new QHexValidator(this));
            }
            else
            {
                const int nums = QString::number(top.toUInt()).length();
                _leadingZeroWidth = qMax(1, nums);
                setMaxLength(qMax(1, nums));
                setValidator(new QUIntValidator(bottom.toUInt(), top.toUInt(), this));
            }
        }
        break;

        case HexMode:
        {
            const int nums = QString::number(top.toUInt(), 16).length();
            _leadingZeroWidth = qMax(1, nums);
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
            if(_hexView)
            {
                _leadingZeroWidth = qMax(1, (int)QString::number(static_cast<quint64>(top.toLongLong()), 16).length());
                setMaxLength(_leadingZeroWidth + 2); // +2 for "0x"
            }
            else
            {
                const int nums = QString::number(top.toLongLong()).length();
                setMaxLength(qMax(2, nums + 1));
                setValidator(new QInt64Validator(bottom.toLongLong(), top.toLongLong(), this));
            }
        }
        break;

        case UInt64Mode:
        {
            if(_hexView)
            {
                _leadingZeroWidth = qMax(1, (int)QString::number(top.toULongLong(), 16).length());
                setMaxLength(_leadingZeroWidth + 2); // +2 for "0x"
            }
            else
            {
                const int nums = QString::number(top.toULongLong()).length();
                _leadingZeroWidth = qMax(1, nums);
                setMaxLength(qMax(1, nums));
                setValidator(new QUIntValidator(bottom.toULongLong(), top.toULongLong(), this));
            }
        }
        break;
    }
    internalSetValue(_value);
    if(!isBlocked) blockSignals(false);
}

///
/// \brief NumericLineEdit::on_hexViewToggled
/// \param on
///
void NumericLineEdit::on_hexViewToggled(bool on)
{
    _hexView = on;
    on_rangeChanged(_minValue, _maxValue);
    emit hexViewChanged(on);
}
