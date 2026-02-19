#ifndef NUMERICCOMBOBOX_H
#define NUMERICCOMBOBOX_H

#include <QComboBox>

///
/// \brief The NumericComboBox class
///
class NumericComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit NumericComboBox(QWidget *parent = nullptr);

    void addValue(int value);
    int currentValue() const;
    void setCurrentValue(int value);
};

#endif // NUMERICCOMBOBOX_H
