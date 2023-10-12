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

    static QVector<QModbusPdu::FunctionCode> validCodes() {
        static const QVector<QModbusPdu::FunctionCode> codes = {
            QModbusPdu::ReadCoils, QModbusPdu::ReadDiscreteInputs, QModbusPdu::ReadHoldingRegisters, QModbusPdu::ReadInputRegisters,
            QModbusPdu::WriteSingleCoil, QModbusPdu::WriteSingleRegister, QModbusPdu::ReadExceptionStatus, QModbusPdu::Diagnostics,
            QModbusPdu::GetCommEventCounter, QModbusPdu::GetCommEventLog, QModbusPdu::WriteMultipleCoils, QModbusPdu::WriteMultipleRegisters,
            QModbusPdu::ReportServerId, QModbusPdu::ReadFileRecord, QModbusPdu::WriteFileRecord, QModbusPdu::MaskWriteRegister,
            QModbusPdu::ReadWriteMultipleRegisters, QModbusPdu::ReadFifoQueue, QModbusPdu::EncapsulatedInterfaceTransport
        };
        return codes;
    }

    bool isValid() const
    {
        return validCodes().contains(_code);
    }

    bool isException() const
    {
        return _code & QModbusPdu::ExceptionByte;
    }

    operator int() const
    {
        return _code;
    }

    operator QString() const
    {
        QString name;
        switch(_code & ~QModbusPdu::ExceptionByte)
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
                break;

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
