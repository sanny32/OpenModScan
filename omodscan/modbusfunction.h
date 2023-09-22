#ifndef MODBUSFUNCTION_H
#define MODBUSFUNCTION_H

#include <QModbusPdu>

///
/// \brief The ModbusFunction class
///
class ModbusFunction
{
public:
    explicit ModbusFunction(QModbusPdu::FunctionCode code)
        :_code(code)
    {
    }

    operator int()
    {
        return _code;
    }

    operator QString()
    {
        QString name;
        switch(_code)
        {
            case QModbusPdu::ReadCoils:
                name = "READ COILS";
                break;

            case QModbusPdu::ReadDiscreteInputs:
                name = "READ INPUTS";
                break;

            case QModbusPdu::ReadHoldingRegisters:
                name = "READ HOLDING REGS";
                break;

            case QModbusPdu::ReadInputRegisters:
                name = "READ INPUT REGS";
                break;

            case QModbusPdu::WriteSingleCoil:
                name = "WRITE SINGLE COIL";
                break;

            case QModbusPdu::WriteSingleRegister:
                name = "WRITE SINGLE REG";
                break;

            case QModbusPdu::ReadExceptionStatus:
                name = "READ EXCEPTION STAT";
                break;

            case QModbusPdu::Diagnostics:
                name = "DIAGNOSTICS";

            case QModbusPdu::GetCommEventCounter:
                name = "GET COMM EVENT CNT";
                break;

            case QModbusPdu::GetCommEventLog:
                name = "GET COMM EVENT LOG";
                break;

            case QModbusPdu::WriteMultipleCoils:
                name = "WRITE MULT COILS";
                break;

            case QModbusPdu::WriteMultipleRegisters:
                name = "WRITE MULT REGS";
                break;

            case QModbusPdu::ReportServerId:
                name = "REPORT SLAVE ID";
                break;

            case QModbusPdu::ReadFileRecord:
                name = "READ FILE RECORD";
                break;

            case QModbusPdu::WriteFileRecord:
                name = "WRITE FILE RECORD";
                break;

            case QModbusPdu::MaskWriteRegister:
                name = "MASK WRITE REG";
                break;

            case QModbusPdu::ReadWriteMultipleRegisters:
                name = "READ WRITE MULT REGS";
                break;

            case QModbusPdu::ReadFifoQueue:
                name ="READ FIFO QUEUE";
                break;

            case QModbusPdu::EncapsulatedInterfaceTransport:
                name = "ENC IFACE TRANSPORT";
                break;

            default:
                break;
        }
        return name;
    }

private:
    QModbusPdu::FunctionCode _code;
};

#endif // MODBUSFUNCTION_H
