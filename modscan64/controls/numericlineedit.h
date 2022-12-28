#ifndef NUMERICLINEEDIT_H
#define NUMERICLINEEDIT_H

#include <QLineEdit>

class NumericLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit NumericLineEdit(QWidget *parent = nullptr);
    explicit NumericLineEdit(const QString &, QWidget *parent = nullptr);

    int value();
    void setValue(int value);

    void enablePaddingZero(bool on);
    void setInputRange(int bottom, int top);

    void setText(const QString& text);

protected:
    void focusOutEvent(QFocusEvent*) override;

private slots:
    void on_editingFinished();

private:
    void updateValue();

private:
    int _value;
    int _paddingZeroWidth;
    bool _enablePaddingZero;
};

#endif // NUMERICLINEEDIT_H
