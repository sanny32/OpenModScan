#include "paritytypecombobox.h"

///
/// \brief ParityTypeComboBox::ParityTypeComboBox
/// \param parent
///
ParityTypeComboBox::ParityTypeComboBox(QWidget* parent)
    :QComboBox(parent)
{
    addItem("ODD", QSerialPort::OddParity);
    addItem("EVEN", QSerialPort::EvenParity);
    addItem("NONE", QSerialPort::NoParity);
}

///
/// \brief ParityTypeComboBox::currentParity
/// \return
///
QSerialPort::Parity ParityTypeComboBox::currentParity() const
{
    return currentData().value<QSerialPort::Parity>();
}

///
/// \brief ParityTypeComboBox::setCurrentParity
/// \param parity
///
void ParityTypeComboBox::setCurrentParity(QSerialPort::Parity parity)
{
    const auto idx = findData(parity);
    setCurrentIndex(idx);
}
