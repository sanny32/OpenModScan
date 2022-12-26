#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>

class NumberLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit NumberLineEdit(QWidget *parent = nullptr);
    explicit NumberLineEdit(const QString &, QWidget *parent = nullptr);

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

#endif // NUMBERLINEEDIT_H
