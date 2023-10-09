#include "modbusmessages.h"


///
/// \brief ModbusMessage::create
/// \param pdu
/// \param timestamp
/// \param deviceId
/// \param request
/// \return
///
const ModbusMessage* ModbusMessage::create(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request)
{
    switch(pdu.functionCode())
    {
        case QModbusPdu::ReadCoils:
            if(request) return new ReadCoilsRequest(pdu, timestamp, deviceId);
            else return new ReadCoilsResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return new ReadDiscreteInputsRequest(pdu, timestamp, deviceId);
            else return new ReadDiscreteInputsResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return new ReadHoldingRegistersRequest(pdu, timestamp, deviceId);
            else return new ReadHoldingRegistersResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadInputRegisters:
            if(request) return new ReadInputRegistersRequest(pdu, timestamp, deviceId);
            else return new ReadInputRegistersResponse(pdu, timestamp, deviceId);

        case QModbusPdu::WriteSingleCoil:
            if(request) return new WriteSingleCoilRequest(pdu, timestamp, deviceId);
            else return new WriteSingleCoilResponse(pdu, timestamp, deviceId);

        case QModbusPdu::WriteSingleRegister:
            if(request) return new WriteSingleRegisterRequest(pdu, timestamp, deviceId);
            else return new WriteSingleRegisterResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadExceptionStatus:
            if(request) return new ReadExceptionStatusRequest(pdu, timestamp, deviceId);
            else return new ReadExceptionStatusResponse(pdu, timestamp, deviceId);

        case QModbusPdu::Diagnostics:
            if(request) return new DiagnosticsRequest(pdu, timestamp, deviceId);
            else return new DiagnosticsResponse(pdu, timestamp, deviceId);

        case QModbusPdu::GetCommEventCounter:
            if(request) return new GetCommEventCounterRequest(pdu, timestamp, deviceId);
            else return new GetCommEventCounterResponse(pdu, timestamp, deviceId);

        case QModbusPdu::GetCommEventLog:
            if(request) return new GetCommEventLogRequest(pdu, timestamp, deviceId);
            else return new GetCommEventLogResponse(pdu, timestamp, deviceId);

        case QModbusPdu::WriteMultipleCoils:
            if(request) return new WriteMultipleCoilsRequest(pdu, timestamp, deviceId);
            else return new WriteMultipleCoilsResponse(pdu, timestamp, deviceId);

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return new WriteMultipleRegistersRequest(pdu, timestamp, deviceId);
            else return new WriteMultipleRegistersResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReportServerId:
            if(request) return new ReportServerIdRequest(pdu, timestamp, deviceId);
            else return new ReportServerIdResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadFileRecord:
            if(request) return new ReadFileRecordRequest(pdu, timestamp, deviceId);
            else return new ReadFileRecordResponse(pdu, timestamp, deviceId);

        case QModbusPdu::WriteFileRecord:
            if(request) return new WriteFileRecordRequest(pdu, timestamp, deviceId);
            else return new WriteFileRecordResponse(pdu, timestamp, deviceId);

        case QModbusPdu::MaskWriteRegister:
            if(request) return new MaskWriteRegisterRequest(pdu, timestamp, deviceId);
            else return new MaskWriteRegisterResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return new ReadWriteMultipleRegistersRequest(pdu, timestamp, deviceId);
            else return new ReadWriteMultipleRegistersResponse(pdu, timestamp, deviceId);

        case QModbusPdu::ReadFifoQueue:
            if(request) return new ReadFifoQueueRequest(pdu, timestamp, deviceId);
            else return new ReadFifoQueueResponse(pdu, timestamp, deviceId);

        default:
            return new ModbusMessage(pdu, timestamp, deviceId, request);
    }
}

///
/// \brief ModbusMessage::create
/// \param adu
/// \param timestamp
/// \param request
/// \return
///
const ModbusMessage* ModbusMessage::create(const QModbusAdu& adu, const QDateTime& timestamp, bool request)
{
    switch(adu.functionCode())
    {
        case QModbusPdu::ReadCoils:
            if(request) return new ReadCoilsRequest(adu, timestamp);
            else return new ReadCoilsResponse(adu, timestamp);

        case QModbusPdu::ReadDiscreteInputs:
            if(request) return new ReadDiscreteInputsRequest(adu, timestamp);
            else return new ReadDiscreteInputsResponse(adu, timestamp);

        case QModbusPdu::ReadHoldingRegisters:
            if(request) return new ReadHoldingRegistersRequest(adu, timestamp);
            else return new ReadHoldingRegistersResponse(adu, timestamp);

        case QModbusPdu::ReadInputRegisters:
            if(request) return new ReadInputRegistersRequest(adu, timestamp);
            else return new ReadInputRegistersResponse(adu, timestamp);

        case QModbusPdu::WriteSingleCoil:
            if(request) return new WriteSingleCoilRequest(adu, timestamp);
            else return new WriteSingleCoilResponse(adu, timestamp);

        case QModbusPdu::WriteSingleRegister:
            if(request) return new WriteSingleRegisterRequest(adu, timestamp);
            else return new WriteSingleRegisterResponse(adu, timestamp);

        case QModbusPdu::ReadExceptionStatus:
            if(request) return new ReadExceptionStatusRequest(adu, timestamp);
            else return new ReadExceptionStatusResponse(adu, timestamp);

        case QModbusPdu::Diagnostics:
            if(request) return new DiagnosticsRequest(adu, timestamp);
            else return new DiagnosticsResponse(adu, timestamp);

        case QModbusPdu::GetCommEventCounter:
            if(request) return new GetCommEventCounterRequest(adu, timestamp);
            else return new GetCommEventCounterResponse(adu, timestamp);

        case QModbusPdu::GetCommEventLog:
            if(request) return new GetCommEventLogRequest(adu, timestamp);
            else return new GetCommEventLogResponse(adu, timestamp);

        case QModbusPdu::WriteMultipleCoils:
            if(request) return new WriteMultipleCoilsRequest(adu, timestamp);
            else return new WriteMultipleCoilsResponse(adu, timestamp);

        case QModbusPdu::WriteMultipleRegisters:
            if(request) return new WriteMultipleRegistersRequest(adu, timestamp);
            else return new WriteMultipleRegistersResponse(adu, timestamp);

        case QModbusPdu::ReportServerId:
            if(request) return new ReportServerIdRequest(adu, timestamp);
            else return new ReportServerIdResponse(adu, timestamp);

        case QModbusPdu::ReadFileRecord:
            if(request) return new ReadFileRecordRequest(adu, timestamp);
            else return new ReadFileRecordResponse(adu, timestamp);

        case QModbusPdu::WriteFileRecord:
            if(request) return new WriteFileRecordRequest(adu, timestamp);
            else return new WriteFileRecordResponse(adu, timestamp);

        case QModbusPdu::MaskWriteRegister:
            if(request) return new MaskWriteRegisterRequest(adu, timestamp);
            else return new MaskWriteRegisterResponse(adu, timestamp);

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(request) return new ReadWriteMultipleRegistersRequest(adu, timestamp);
            else return new ReadWriteMultipleRegistersResponse(adu, timestamp);

        case QModbusPdu::ReadFifoQueue:
            if(request) return new ReadFifoQueueRequest(adu, timestamp);
            else return new ReadFifoQueueResponse(adu, timestamp);

        default:
            return new ModbusMessage(adu, timestamp, request);
    }
}

///
/// \brief ModbusMessage::parse
/// \param data
/// \param type
/// \param protocol
/// \param request
/// \return
///
const ModbusMessage* ModbusMessage::parse(const QByteArray& data, Type type, QModbusAdu::Type protocol, bool request)
{
    switch(type)
    {
        case Adu:
        {
            QModbusAdu adu(protocol, data);
            return create(adu, QDateTime::currentDateTime(), request);
        }

        case Pdu:
        {
            QModbusPdu pdu;
            pdu.setFunctionCode((QModbusPdu::FunctionCode)data[1]);
            pdu.setData(data.mid(2, data.size() - 2));

            return create(pdu, QDateTime::currentDateTime(), data[0], request);
        }

        default:
        return nullptr;
    }
}
