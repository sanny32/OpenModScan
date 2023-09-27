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

        default:
            return nullptr;
    }
}
