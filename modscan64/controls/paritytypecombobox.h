#ifndef PARITYTYPECOMBOBOX_H
#define PARITYTYPECOMBOBOX_H

#include <QComboBox>
#include <QSerialPort>

///
/// \brief The ParityTypeComboBox class
///
class ParityTypeComboBox : public QComboBox
{
    Q_OBJECT
public:
    ParityTypeComboBox(QWidget *parent = nullptr);

    QSerialPort::Parity currentParity() const;
    void setCurrentParity(QSerialPort::Parity parity);
};

#endif // PARITYTYPECOMBOBOX_H
