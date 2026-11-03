#include "flowcontroltypecombobox.h"

///
/// \brief FlowControlTypeComboBox::FlowControlTypeComboBox
/// \param parent
///
FlowControlTypeComboBox::FlowControlTypeComboBox(QWidget* parent)
    :QComboBox(parent)
{
    addItem(tr("NO CONTROL"), QSerialPort::NoFlowControl);
    addItem(tr("HARDWARE (RTS/CTS)"), QSerialPort::HardwareControl);
    addItem(tr("SOFTWARE (XON/XOFF)"), QSerialPort::SoftwareControl);
}

///
/// \brief FlowControlTypeComboBox::currentFlowControl
/// \return
///
QSerialPort::FlowControl FlowControlTypeComboBox::currentFlowControl() const
{
    return currentData().value<QSerialPort::FlowControl>();
}

///
/// \brief FlowControlTypeComboBox::setCurrentFlowControl
/// \param fctrl
///
void FlowControlTypeComboBox::setCurrentFlowControl(QSerialPort::FlowControl fctrl)
{
    const auto idx = findData(fctrl);
    setCurrentIndex(idx);
}
