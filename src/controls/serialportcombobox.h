#ifndef SERIALPORTCOMBOBOX_H
#define SERIALPORTCOMBOBOX_H

#include <QComboBox>
#include "enums.h"

///
/// \brief The SerialPortComboBox class
///
class SerialPortComboBox : public QComboBox
{
    Q_OBJECT
public:
    SerialPortComboBox(QWidget* parent = nullptr);

    bool excludeVirtuals() const;
    void setExcludeVirtuals(bool exclude);

    QString currentPortName() const;

private:
    bool _excludeVirtuals = false;
};

#endif // SERIALPORTCOMBOBOX_H
