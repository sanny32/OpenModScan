#include "modbusprotocolscombobox.h"

///
/// \brief ModbusProtocolsComboBox::ModbusProtocolsComboBox
/// \param parent
///
ModbusProtocolsComboBox::ModbusProtocolsComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItem("Modbus TCP/IP", ModbusProtocol::ModbusTcp);
    addItem("Modbus RTU/IP", ModbusProtocol::ModbusRtuTcp);
    addItem("Modbus RTU", ModbusProtocol::ModbusRtu);

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ModbusProtocolsComboBox::on_currentIndexChanged);
}

///
/// \brief ModbusProtocolsComboBox::currentModbusProtocol
/// \return
///
ModbusProtocolsComboBox::ModbusProtocol ModbusProtocolsComboBox::currentModbusProtocol() const
{
    return currentData().value<ModbusProtocolsComboBox::ModbusProtocol>();
}

///
/// \brief ModbusProtocolsComboBox::setCurrentModbusProtocol
/// \param mbp
///
void ModbusProtocolsComboBox::setCurrentModbusProtocol(ModbusProtocol mbp)
{
    const auto idx = findData(mbp);
    setCurrentIndex(idx);
}


///
/// \brief ModbusProtocolsComboBox::on_currentIndexChanged
/// \param index
///
void ModbusProtocolsComboBox::on_currentIndexChanged(int index)
{
    emit modbusProtocolChanged(itemData(index).value<ModbusProtocol>());
}
