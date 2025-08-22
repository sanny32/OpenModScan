#include "modbusmessages.h"


///
/// \brief ModbusMessage::create
/// \param pdu
/// \param protocol
/// \param deviceId
/// \param timestamp
/// \param request
/// \return
///
QSharedPointer<const ModbusMessage> ModbusMessage::create(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp, bool request)
{
    switch(pdu.functionCode())
    {
        case QModbusPdu::ReadCoils:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadCoilsRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadCoilsResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadDiscreteInputsRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadDiscreteInputsResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadHoldingRegistersRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadHoldingRegistersResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadInputRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadInputRegistersRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadInputRegistersResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::WriteSingleCoil:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteSingleCoilRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteSingleCoilResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::WriteSingleRegister:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteSingleRegisterRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteSingleRegisterResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadExceptionStatus:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadExceptionStatusRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadExceptionStatusResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::Diagnostics:
            if(request) return QSharedPointer<const ModbusMessage>(new DiagnosticsRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new DiagnosticsResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::GetCommEventCounter:
            if(request) return QSharedPointer<const ModbusMessage>(new GetCommEventCounterRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new GetCommEventCounterResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::GetCommEventLog:
            if(request) return QSharedPointer<const ModbusMessage>(new GetCommEventLogRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new GetCommEventLogResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::WriteMultipleCoils:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteMultipleCoilsRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteMultipleCoilsResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteMultipleRegistersRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteMultipleRegistersResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReportServerId:
            if(request) return QSharedPointer<const ModbusMessage>(new ReportServerIdRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReportServerIdResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadFileRecord:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadFileRecordRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadFileRecordResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::WriteFileRecord:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteFileRecordRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteFileRecordResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::MaskWriteRegister:
            if(request) return QSharedPointer<const ModbusMessage>(new MaskWriteRegisterRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new MaskWriteRegisterResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadWriteMultipleRegistersRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadWriteMultipleRegistersResponse(pdu, protocol, deviceId, timestamp));

        case QModbusPdu::ReadFifoQueue:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadFifoQueueRequest(pdu, protocol, deviceId, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadFifoQueueResponse(pdu, protocol, deviceId, timestamp));

        default:
            return QSharedPointer<const ModbusMessage>(new ModbusMessage(pdu, protocol, deviceId, timestamp, request));
    }
}

///
/// \brief ModbusMessage::create
/// \param data
/// \param protocol
/// \param timestamp
/// \param request
/// \return
///
QSharedPointer<const ModbusMessage> ModbusMessage::create(const QByteArray& data, ProtocolType protocol,  const QDateTime& timestamp, bool request)
{
    QModbusPdu::FunctionCode fc = QModbusPdu::Invalid;
    switch(protocol)
    {
        case Rtu:
            fc = QModbusAduRtu(data).functionCode();
            break;

        case Tcp:
            fc = QModbusAduTcp(data).functionCode();
            break;
    }

    switch(fc)
    {
        case QModbusPdu::ReadCoils:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadCoilsRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadCoilsResponse(data, protocol, timestamp));

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadDiscreteInputsRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadDiscreteInputsResponse(data, protocol, timestamp));

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadHoldingRegistersRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadHoldingRegistersResponse(data, protocol, timestamp));

        case QModbusPdu::ReadInputRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadInputRegistersRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadInputRegistersResponse(data, protocol, timestamp));

        case QModbusPdu::WriteSingleCoil:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteSingleCoilRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteSingleCoilResponse(data, protocol, timestamp));

        case QModbusPdu::WriteSingleRegister:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteSingleRegisterRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteSingleRegisterResponse(data, protocol, timestamp));

        case QModbusPdu::ReadExceptionStatus:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadExceptionStatusRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadExceptionStatusResponse(data, protocol, timestamp));

        case QModbusPdu::Diagnostics:
            if(request) return QSharedPointer<const ModbusMessage>(new DiagnosticsRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new DiagnosticsResponse(data, protocol, timestamp));

        case QModbusPdu::GetCommEventCounter:
            if(request) return QSharedPointer<const ModbusMessage>(new GetCommEventCounterRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new GetCommEventCounterResponse(data, protocol, timestamp));

        case QModbusPdu::GetCommEventLog:
            if(request) return QSharedPointer<const ModbusMessage>(new GetCommEventLogRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new GetCommEventLogResponse(data, protocol, timestamp));

        case QModbusPdu::WriteMultipleCoils:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteMultipleCoilsRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteMultipleCoilsResponse(data, protocol, timestamp));

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteMultipleRegistersRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteMultipleRegistersResponse(data, protocol, timestamp));

        case QModbusPdu::ReportServerId:
            if(request) return QSharedPointer<const ModbusMessage>(new ReportServerIdRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReportServerIdResponse(data, protocol, timestamp));

        case QModbusPdu::ReadFileRecord:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadFileRecordRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadFileRecordResponse(data, protocol, timestamp));

        case QModbusPdu::WriteFileRecord:
            if(request) return QSharedPointer<const ModbusMessage>(new WriteFileRecordRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new WriteFileRecordResponse(data, protocol, timestamp));

        case QModbusPdu::MaskWriteRegister:
            if(request) return QSharedPointer<const ModbusMessage>(new MaskWriteRegisterRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new MaskWriteRegisterResponse(data, protocol, timestamp));

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadWriteMultipleRegistersRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadWriteMultipleRegistersResponse(data, protocol, timestamp));

        case QModbusPdu::ReadFifoQueue:
            if(request) return QSharedPointer<const ModbusMessage>(new ReadFifoQueueRequest(data, protocol, timestamp));
            else return QSharedPointer<const ModbusMessage>(new ReadFifoQueueResponse(data, protocol, timestamp));

        default:
            return QSharedPointer<const ModbusMessage>(new ModbusMessage(data, protocol, timestamp, request));
    }
}
