#ifndef MODBUSEXCEPTION_H
#define MODBUSEXCEPTION_H

#include <QModbusPdu>

///
/// \brief The ModbusException class
///
class ModbusException
{
public:
    explicit ModbusException(QModbusPdu::ExceptionCode code)
        :_code(code)
    {
    }

    operator QString()
    {
        QString desc;
        switch (_code)
        {
            case QModbusPdu::IllegalFunction:
                desc = "ILLEGAL FUNCTION";
            break;
            case QModbusPdu::IllegalDataAddress:
                desc = "ILLEGAL DATA ADDRESS";
            break;
            case QModbusPdu::IllegalDataValue:
                desc = "ILLEGAL DATA VALUE";
            break;
            case QModbusPdu::ServerDeviceFailure:
                desc = "SERVER DEVICE FAILURE";
            break;
            case QModbusPdu::Acknowledge:
                desc = "ACKNOWLEDGE";
            break;
            case QModbusPdu::ServerDeviceBusy:
                desc = "SERVER DEVICE BUSY";
            break;
            case QModbusPdu::NegativeAcknowledge:
                desc = "NEGATIVE ACKNOWLEDGEMENT";
            break;
            case QModbusPdu::MemoryParityError:
                desc = "MEMORY PARITY ERROR";
            break;
            case QModbusPdu::GatewayPathUnavailable:
                desc = "GATEWAY PATH UNAVAILABLE";
            break;
            case QModbusPdu::GatewayTargetDeviceFailedToRespond:
                desc = "GATEWAY TARGET DEVICE FAILED TO RESPOND";
            break;
            case QModbusPdu::ExtendedException:
                desc = "EXTENDED EXCEPTION";
            break;
        }
        return QString("%1 (0x%2)").arg(desc, QString::number(_code, 16));
    }

private:
    QModbusPdu::ExceptionCode _code;
};

#endif // MODBUSEXCEPTION_H
