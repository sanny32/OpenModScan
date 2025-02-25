#ifndef NUMERICLINEEDIT_H
#define NUMERICLINEEDIT_H

#include <QLineEdit>
#include "qrange.h"

class NumericLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode)
    Q_PROPERTY(bool paddingZeroes READ paddingZeroes WRITE setPaddingZeroes)

public:
    enum InputMode
    {
        Int32Mode = 0,
        UInt32Mode,
        HexMode,
        FloatMode,
        DoubleMode,
        Int64Mode,
        UInt64Mode,
        AnsiMode
    };

    explicit NumericLineEdit(QWidget* parent = nullptr);
    explicit NumericLineEdit(InputMode mode, QWidget *parent = nullptr);

    template<typename T>
    T value() const { return _value.value<T>(); }

    template<typename T>
    void setValue(T value)
    { internalSetValue(value); }

    bool paddingZeroes() const;
    void setPaddingZeroes(bool on);

    template<typename T>
    QRange<T> range() const
    { return { _minValue.value<T>(), _maxValue.value<T>() }; }

    template<typename T>
    void setInputRange(QRange<T> range)
    { setInputRange(range.from(), range.to()); }

    template<typename T>
    void setInputRange(T bottom, T top)
    {
        _minValue = QVariant::fromValue<T>(bottom);
        _maxValue = QVariant::fromValue<T>(top);
        emit rangeChanged(_minValue, _maxValue);
    }

    InputMode inputMode() const;
    void setInputMode(InputMode mode);

    QString codepage() const;
    void setCodepage(const QString& name);

    void setText(const QString& text);

signals:
    void valueChanged(const QVariant& value);
    void rangeChanged(const QVariant& bottom, const QVariant& top);

protected:
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void on_editingFinished();
    void on_textChanged(const QString& text);
    void on_rangeChanged(const QVariant& bottom, const QVariant& top);

private:
    void updateValue();
    void internalSetValue(QVariant value);

private:
    QVariant _value;
    QVariant _minValue;
    QVariant _maxValue;
    InputMode _inputMode;
    bool _paddingZeroes;
    int _paddingZeroWidth;
    QString _codepage;
};

#endif // NUMERICLINEEDIT_H
