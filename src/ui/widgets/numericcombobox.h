#ifndef NUMERICCOMBOBOX_H
#define NUMERICCOMBOBOX_H

#include <QComboBox>
#include "numericlineedit.h"

///
/// \brief The NumericComboBox class
/// An editable combo box that accepts only integer values.
/// Emits currentValueChanged when the selected or entered value changes.
///
class NumericComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)

public:
    explicit NumericComboBox(QWidget *parent = nullptr);

    void addValue(int value);
    int currentValue() const;
    void setCurrentValue(int value);

    int minimum() const;
    void setMinimum(int min);

    int maximum() const;
    void setMaximum(int max);

    void setRange(int min, int max);

signals:
    void currentValueChanged(int value);

protected:
    void focusOutEvent(QFocusEvent*) override;

private slots:
    void onCurrentTextChanged(const QString& text);

private:
    NumericLineEdit* _lineEdit;
};

#endif // NUMERICCOMBOBOX_H
