#ifndef NUMERICCOMBOBOX_H
#define NUMERICCOMBOBOX_H

#include <QComboBox>

class NumericComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit NumericComboBox(QWidget *parent = nullptr);

    void addValue(int value);
    int currentValue() const;
};

#endif // NUMERICCOMBOBOX_H
