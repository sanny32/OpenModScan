#ifndef NUMERICLINEEDIT_H
#define NUMERICLINEEDIT_H

#include <QLineEdit>
#include "qrange.h"

class QToolButton;

class NumericLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode)
    Q_PROPERTY(bool leadingZeroes READ leadingZeroes WRITE setLeadingZeroes)
    Q_PROPERTY(bool hexView READ hexView WRITE setHexView)
    Q_PROPERTY(bool hexButtonVisible READ hexButtonVisible WRITE setHexButtonVisible)
    Q_PROPERTY(bool allowEmptyValue READ allowEmptyValue WRITE setAllowEmptyValue)

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

    bool leadingZeroes() const;
    void setLeadingZeroes(bool on);

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
        on_rangeChanged(_minValue, _maxValue);
    }

    InputMode inputMode() const;
    void setInputMode(InputMode mode);

    QString codepage() const;
    void setCodepage(const QString& name);

    void setText(const QString& text);

    bool hexView() const;
    void setHexView(bool on);

    bool hexButtonVisible() const;
    void setHexButtonVisible(bool visible);

    bool allowEmptyValue() const;
    void setAllowEmptyValue(bool allow);

    bool isEmpty() const;
    void clearValue();

signals:
    void valueChanged(const QVariant& newValue);
    void valueChanged(const QVariant& oldValue, const QVariant& newValue);
    void rangeChanged(const QVariant& bottom, const QVariant& top);
    void hexViewChanged(bool on);

protected:
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void on_editingFinished();
    void on_textChanged(const QString& text);
    void on_rangeChanged(const QVariant& bottom, const QVariant& top);
    void on_hexViewToggled(bool on);

private:
    void updateValue();
    void internalSetValue(QVariant value);
    void updateButtons();
    bool isHexViewApplicable() const;

private:
    QVariant _value;
    QVariant _minValue;
    QVariant _maxValue;
    InputMode _inputMode;
    bool _leadingZeroes;
    int _leadingZeroWidth;
    QString _codepage;
    QToolButton* _hexButton;
    bool _hexView;
    bool _hexButtonVisible;
    bool _allowEmptyValue;
};

#endif // NUMERICLINEEDIT_H
