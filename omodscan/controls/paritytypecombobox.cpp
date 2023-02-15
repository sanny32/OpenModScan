#include "paritytypecombobox.h"

///
/// \brief ParityTypeComboBox::ParityTypeComboBox
/// \param parent
///
ParityTypeComboBox::ParityTypeComboBox(QWidget* parent)
    :QComboBox(parent)
{
    addItem(tr("ODD"), QSerialPort::OddParity);
    addItem(tr("EVEN"), QSerialPort::EvenParity);
    addItem(tr("NONE"), QSerialPort::NoParity);
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
