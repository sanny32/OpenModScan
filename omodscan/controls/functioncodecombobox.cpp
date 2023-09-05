#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    :QComboBox(parent)
{
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
/// \brief FunctionCodeComboBox::addItem
/// \param funcCode
///
void FunctionCodeComboBox::addItem(QModbusPdu::FunctionCode funcCode)
{
    switch(funcCode)
    {
        case QModbusPdu::ReadCoils:
            QComboBox::addItem("01: READ COILS", QModbusPdu::ReadCoils);
        break;

        case QModbusPdu::ReadDiscreteInputs:
            QComboBox::addItem("02: READ INPUTS", QModbusPdu::ReadDiscreteInputs);
        break;

        case QModbusPdu::ReadHoldingRegisters:
            QComboBox::addItem("03: READ HOLDING REGS", QModbusPdu::ReadHoldingRegisters);
        break;

        case QModbusPdu::ReadInputRegisters:
            QComboBox::addItem("04: READ INPUT REGS", QModbusPdu::ReadInputRegisters);
        break;

        case QModbusPdu::WriteSingleCoil:
            QComboBox::addItem("05: WRITE SINGLE COIL", QModbusPdu::WriteSingleCoil);
        break;

        case QModbusPdu::WriteSingleRegister:
            QComboBox::addItem("06: WRITE SINGLE REG", QModbusPdu::WriteSingleRegister);
        break;

        case QModbusPdu::ReadExceptionStatus:
            QComboBox::addItem("07: READ EXCEPTION STAT", QModbusPdu::ReadExceptionStatus);
        break;

        case QModbusPdu::Diagnostics:
            QComboBox::addItem("08: DIAGNOSTICS", QModbusPdu::Diagnostics);
        break;

        case QModbusPdu::GetCommEventCounter:
            QComboBox::addItem("11: GET COMM EVENT CNT", QModbusPdu::GetCommEventCounter);
        break;

        case QModbusPdu::GetCommEventLog:
            QComboBox::addItem("12: GET COMM EVENT LOG", QModbusPdu::GetCommEventLog);
        break;

        case QModbusPdu::WriteMultipleCoils:
            QComboBox::addItem("15: WRITE MULT COILS", QModbusPdu::WriteMultipleCoils);
        break;

        case QModbusPdu::WriteMultipleRegisters:
            QComboBox::addItem("16: WRITE MULT REGS", QModbusPdu::WriteMultipleRegisters);
        break;

        case QModbusPdu::ReportServerId:
            QComboBox::addItem("17: REPORT SLAVE ID", QModbusPdu::ReportServerId);
        break;

        case QModbusPdu::ReadFileRecord:
            QComboBox::addItem("20: READ FILE RECORD", QModbusPdu::ReadFileRecord);
        break;

        case QModbusPdu::WriteFileRecord:
            QComboBox::addItem("21: WRITE FILE RECORD", QModbusPdu::WriteFileRecord);
        break;

        case QModbusPdu::MaskWriteRegister:
            QComboBox::addItem("22: MASK WRITE REG", QModbusPdu::MaskWriteRegister);
        break;

        case QModbusPdu::ReadWriteMultipleRegisters:
            QComboBox::addItem("23: READ WRITE MULT REGS", QModbusPdu::ReadWriteMultipleRegisters);
        break;

        case QModbusPdu::ReadFifoQueue:
            QComboBox::addItem("24: READ FIFO QUEUE", QModbusPdu::ReadFifoQueue);
        break;

        case QModbusPdu::EncapsulatedInterfaceTransport:
            QComboBox::addItem("43: ENC IFACE TRANSPORT", QModbusPdu::EncapsulatedInterfaceTransport);
        break;

        default:
        break;
    }
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    emit functionCodeChanged(itemData(index).value<QModbusPdu::FunctionCode>());
}
