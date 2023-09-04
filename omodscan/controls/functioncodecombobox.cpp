#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem("01: READ COILS", QModbusPdu::ReadCoils);
    addItem("02: READ INPUTS", QModbusPdu::ReadDiscreteInputs);
    addItem("03: READ HOLDING REGS", QModbusPdu::ReadHoldingRegisters);
    addItem("04: READ INPUT REGS", QModbusPdu::ReadInputRegisters);
    addItem("17: REPORT SLAVE ID", QModbusPdu::ReportServerId);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
}

///
/// \brief FunctionCodeComboBox::currentFunctionCode
/// \return
///
QModbusPdu::FunctionCode FunctionCodeComboBox::currentFunctionCode() const
{
    return currentData().value<QModbusPdu::FunctionCode>();
}

///
/// \brief FunctionCodeComboBox::setCurrentFunctionCode
/// \param pointType
///
void FunctionCodeComboBox::setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode)
{
    const auto idx = findData(funcCode);
    setCurrentIndex(idx);
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    emit functionCodeChanged(itemData(index).value<QModbusPdu::FunctionCode>());
}
