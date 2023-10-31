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
const ModbusMessage* ModbusMessage::create(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp, bool request)
{
    switch(pdu.functionCode())
    {
        case QModbusPdu::ReadCoils:
            if(request) return new ReadCoilsRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadCoilsResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return new ReadDiscreteInputsRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadDiscreteInputsResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return new ReadHoldingRegistersRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadHoldingRegistersResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadInputRegisters:
            if(request) return new ReadInputRegistersRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadInputRegistersResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::WriteSingleCoil:
            if(request) return new WriteSingleCoilRequest(pdu, protocol, deviceId, timestamp);
            else return new WriteSingleCoilResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::WriteSingleRegister:
            if(request) return new WriteSingleRegisterRequest(pdu, protocol, deviceId, timestamp);
            else return new WriteSingleRegisterResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadExceptionStatus:
            if(request) return new ReadExceptionStatusRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadExceptionStatusResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::Diagnostics:
            if(request) return new DiagnosticsRequest(pdu, protocol, deviceId, timestamp);
            else return new DiagnosticsResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::GetCommEventCounter:
            if(request) return new GetCommEventCounterRequest(pdu, protocol, deviceId, timestamp);
            else return new GetCommEventCounterResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::GetCommEventLog:
            if(request) return new GetCommEventLogRequest(pdu, protocol, deviceId, timestamp);
            else return new GetCommEventLogResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::WriteMultipleCoils:
            if(request) return new WriteMultipleCoilsRequest(pdu, protocol, deviceId, timestamp);
            else return new WriteMultipleCoilsResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return new WriteMultipleRegistersRequest(pdu, protocol, deviceId, timestamp);
            else return new WriteMultipleRegistersResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReportServerId:
            if(request) return new ReportServerIdRequest(pdu, protocol, deviceId, timestamp);
            else return new ReportServerIdResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadFileRecord:
            if(request) return new ReadFileRecordRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadFileRecordResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::WriteFileRecord:
            if(request) return new WriteFileRecordRequest(pdu, protocol, deviceId, timestamp);
            else return new WriteFileRecordResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::MaskWriteRegister:
            if(request) return new MaskWriteRegisterRequest(pdu, protocol, deviceId, timestamp);
            else return new MaskWriteRegisterResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return new ReadWriteMultipleRegistersRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadWriteMultipleRegistersResponse(pdu, protocol, deviceId, timestamp);

        case QModbusPdu::ReadFifoQueue:
            if(request) return new ReadFifoQueueRequest(pdu, protocol, deviceId, timestamp);
            else return new ReadFifoQueueResponse(pdu, protocol, deviceId, timestamp);

        default:
            return new ModbusMessage(pdu, protocol, deviceId, timestamp, request);
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
const ModbusMessage* ModbusMessage::create(const QByteArray& data, ProtocolType protocol,  const QDateTime& timestamp, bool request)
{
    QModbusPdu::FunctionCode fc;
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
            if(request) return new ReadCoilsRequest(data, protocol, timestamp);
            else return new ReadCoilsResponse(data, protocol, timestamp);

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return new ReadDiscreteInputsRequest(data, protocol, timestamp);
            else return new ReadDiscreteInputsResponse(data, protocol, timestamp);

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return new ReadHoldingRegistersRequest(data, protocol, timestamp);
            else return new ReadHoldingRegistersResponse(data, protocol, timestamp);

        case QModbusPdu::ReadInputRegisters:
            if(request) return new ReadInputRegistersRequest(data, protocol, timestamp);
            else return new ReadInputRegistersResponse(data, protocol, timestamp);

        case QModbusPdu::WriteSingleCoil:
            if(request) return new WriteSingleCoilRequest(data, protocol, timestamp);
            else return new WriteSingleCoilResponse(data, protocol, timestamp);

        case QModbusPdu::WriteSingleRegister:
            if(request) return new WriteSingleRegisterRequest(data, protocol, timestamp);
            else return new WriteSingleRegisterResponse(data, protocol, timestamp);

        case QModbusPdu::ReadExceptionStatus:
            if(request) return new ReadExceptionStatusRequest(data, protocol, timestamp);
            else return new ReadExceptionStatusResponse(data, protocol, timestamp);

        case QModbusPdu::Diagnostics:
            if(request) return new DiagnosticsRequest(data, protocol, timestamp);
            else return new DiagnosticsResponse(data, protocol, timestamp);

        case QModbusPdu::GetCommEventCounter:
            if(request) return new GetCommEventCounterRequest(data, protocol, timestamp);
            else return new GetCommEventCounterResponse(data, protocol, timestamp);

        case QModbusPdu::GetCommEventLog:
            if(request) return new GetCommEventLogRequest(data, protocol, timestamp);
            else return new GetCommEventLogResponse(data, protocol, timestamp);

        case QModbusPdu::WriteMultipleCoils:
            if(request) return new WriteMultipleCoilsRequest(data, protocol, timestamp);
            else return new WriteMultipleCoilsResponse(data, protocol, timestamp);

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return new WriteMultipleRegistersRequest(data, protocol, timestamp);
            else return new WriteMultipleRegistersResponse(data, protocol, timestamp);

        case QModbusPdu::ReportServerId:
            if(request) return new ReportServerIdRequest(data, protocol, timestamp);
            else return new ReportServerIdResponse(data, protocol, timestamp);

        case QModbusPdu::ReadFileRecord:
            if(request) return new ReadFileRecordRequest(data, protocol, timestamp);
            else return new ReadFileRecordResponse(data, protocol, timestamp);

        case QModbusPdu::WriteFileRecord:
            if(request) return new WriteFileRecordRequest(data, protocol, timestamp);
            else return new WriteFileRecordResponse(data, protocol, timestamp);

        case QModbusPdu::MaskWriteRegister:
            if(request) return new MaskWriteRegisterRequest(data, protocol, timestamp);
            else return new MaskWriteRegisterResponse(data, protocol, timestamp);

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return new ReadWriteMultipleRegistersRequest(data, protocol, timestamp);
            else return new ReadWriteMultipleRegistersResponse(data, protocol, timestamp);

        case QModbusPdu::ReadFifoQueue:
            if(request) return new ReadFifoQueueRequest(data, protocol, timestamp);
            else return new ReadFifoQueueResponse(data, protocol, timestamp);

        default:
            return new ModbusMessage(data, protocol, timestamp, request);
    }
}
