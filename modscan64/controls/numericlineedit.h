#ifndef NUMERICLINEEDIT_H
#define NUMERICLINEEDIT_H

#include <QLineEdit>
#include "qrange.h"

class NumericLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool hexInput READ hexInput WRITE setHexInput)
    Q_PROPERTY(bool paddingZeroes READ paddingZeroes WRITE setPaddingZeroes)

public:
    explicit NumericLineEdit(QWidget* parent = nullptr);
    explicit NumericLineEdit(const QString&, QWidget *parent = nullptr);

    int value();
    void setValue(int value);

    bool paddingZeroes() const;
    void setPaddingZeroes(bool on);

    QRange<int> range() const;
    void setInputRange(QRange<int> range);
    void setInputRange(int bottom, int top);

    bool hexInput() const;
    void setHexInput(bool on);

    void setText(const QString& text);

signals:
    void valueChanged(int value);

protected:
    void focusOutEvent(QFocusEvent*) override;

private slots:
    void on_editingFinished();
    void on_textChanged(const QString& text);

private:
    void updateValue();
    void internalSetValue(int value);

private:
    int _value;
    int _paddingZeroWidth;
    bool _paddingZeroes;
    bool _hexInput;
};

#endif // NUMERICLINEEDIT_H
