#ifndef NUMERICLINEEDIT_H
#define NUMERICLINEEDIT_H

#include <QLineEdit>

class NumericLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool paddingZeroes READ paddingZeroes WRITE setPaddingZeroes)

public:
    explicit NumericLineEdit(QWidget* parent = nullptr);
    explicit NumericLineEdit(const QString&, QWidget *parent = nullptr);

    int value();
    void setValue(int value);

    bool paddingZeroes() const;
    void setPaddingZeroes(bool on);

    void setInputRange(int bottom, int top);
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

private:
    int _value;
    int _paddingZeroWidth;
    bool _paddingZeroes;
};

#endif // NUMERICLINEEDIT_H
