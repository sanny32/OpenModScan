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

protected:
    void focusOutEvent(QFocusEvent*) override;

private slots:
    void on_textChanged(const QString& s);

private:
    int _value;
    int _paddingZeroWidth;
    bool _enablePaddingZero;
};

#endif // NUMERICLINEEDIT_H
