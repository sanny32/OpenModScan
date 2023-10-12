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

signals:
    void parityTypeChanged(QSerialPort::Parity parity);

private slots:
    void on_currentIndexChanged(int index);
};

#endif // PARITYTYPECOMBOBOX_H
