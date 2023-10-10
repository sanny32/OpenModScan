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
const ModbusMessage* ModbusMessage::create(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, bool request)
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

            switch(protocol)
            {
                case QModbusAdu::Tcp:
                    pdu.setData(data.mid(2, data.size() - 2));
                    break;

                case QModbusAdu::Rtu:
                    pdu.setData(data.mid(2, data.size() - 4));
                    break;
            }

            return create(pdu, protocol, data[0], QDateTime::currentDateTime(), request);
        }

        default:
        return nullptr;
    }
}
