#include "modbuspduinfo.h"
#include "readcoils.h"

///
/// \brief ModbusPduInfo::create
/// \param pdu
/// \param timestamp
/// \param deviceId
/// \param request
/// \return
///
const ModbusPduInfo* ModbusPduInfo::create(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request, QObject* parent)
{
    switch(pdu.functionCode())
    {
        case QModbusPdu::ReadCoils:
            if(request) return new ReadCoilsRequest(pdu, timestamp, deviceId, parent);
            else return new ReadCoilsResponse(pdu, timestamp, deviceId, parent);

        default:
            return nullptr;
    }
}
