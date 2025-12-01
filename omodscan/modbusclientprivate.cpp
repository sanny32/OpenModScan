#include <QModbusDevice>
#include <QModbusClient>
#include "modbusclientprivate.h"

Q_LOGGING_CATEGORY(QT_MODBUS, "qt.modbus")
Q_LOGGING_CATEGORY(QT_MODBUS_LOW, "qt.modbus.lowlevel")

///
/// \brief ModbusClientPrivate::ModbusClientPrivate
/// \param parent
///
ModbusClientPrivate::ModbusClientPrivate(QObject *parent)
    : QObject{parent}
{
}

///
/// \brief ModbusClientPrivate::connectDevice
/// \return
///
bool ModbusClientPrivate::connectDevice()
{
    if (_state != ModbusDevice::UnconnectedState)
        return false;

    setState(ModbusDevice::ConnectingState);

    if (!open()) {
        setState(ModbusDevice::UnconnectedState);
        return false;
    }

    //Connected is set by backend -> might be delayed by event loop
    return true;
}

///
/// \brief ModbusClientPrivate::disconnectDevice
///
void ModbusClientPrivate::disconnectDevice()
{
    if (auto s = state(); s == ModbusDevice::UnconnectedState || s == ModbusDevice::ClosingState)
        return;

    setState(ModbusDevice::ClosingState);

    //Unconnected is set by backend -> might be delayed by event loop
    close();
}

///
/// \brief ModbusClientPrivate::state
/// \return
///
ModbusDevice::State ModbusClientPrivate::state() const
{
    return _state;
}

///
/// \brief ModbusClientPrivate::error
/// \return
///
ModbusDevice::Error ModbusClientPrivate::error() const
{
    return _error;
}

///
/// \brief ModbusClientPrivate::errorString
/// \return
///
QString ModbusClientPrivate::errorString() const
{
    return _errorString;
}

///
/// \brief ModbusClientPrivate::timeout
/// \return
///
int ModbusClientPrivate::timeout() const
{
    return _responseTimeoutDuration;
}

///
/// \brief ModbusClientPrivate::setTimeout
/// \param newTimeout
///
void ModbusClientPrivate::setTimeout(int newTimeout)
{
    if (newTimeout < 10)
        return;

    if (_responseTimeoutDuration != newTimeout) {
        _responseTimeoutDuration = newTimeout;
        emit timeoutChanged(newTimeout);
    }
}

///
/// \brief ModbusClientPrivate::numberOfRetries
/// \return
///
int ModbusClientPrivate::numberOfRetries() const
{
    return _numberOfRetries;
}

///
/// \brief ModbusClientPrivate::setNumberOfRetries
/// \param number
///
void ModbusClientPrivate::setNumberOfRetries(int number)
{
    if (number >= 0)
        _numberOfRetries = number;
}

///
/// \brief ModbusClientPrivate::sendReadRequest
/// \param read
/// \param serverAddress
/// \param requestGroupId
/// \return
///
ModbusReply* ModbusClientPrivate::sendReadRequest(const QModbusDataUnit& read, int serverAddress, int requestGroupId)
{
    return sendRequest(requestGroupId, createReadRequest(read), serverAddress, &read);
}

///
/// \brief ModbusClientPrivate::sendWriteRequest
/// \param write
/// \param serverAddress
/// \param requestGroupId
/// \return
///
ModbusReply* ModbusClientPrivate::sendWriteRequest(const QModbusDataUnit& write, int serverAddress, int requestGroupId)
{
    return sendRequest(requestGroupId, createWriteRequest(write), serverAddress, &write);
}

///
/// \brief ModbusClientPrivate::sendReadWriteRequest
/// \param read
/// \param write
/// \param serverAddress
/// \param requestGroupId
/// \return
///
ModbusReply* ModbusClientPrivate::sendReadWriteRequest(const QModbusDataUnit& read, const QModbusDataUnit& write, int serverAddress, int requestGroupId)
{
    return sendRequest(requestGroupId, createRWRequest(read, write), serverAddress, &read);
}

///
/// \brief ModbusClientPrivate::sendRawRequest
/// \param request
/// \param serverAddress
/// \param requestGroupId
/// \return
///
ModbusReply* ModbusClientPrivate::sendRawRequest(const QModbusRequest& request, int serverAddress, int requestGroupId)
{
    return sendRequest(requestGroupId, request, serverAddress, nullptr);
}

///
/// \brief ModbusClientPrivate::setState
/// \param newState
///
void ModbusClientPrivate::setState(ModbusDevice::State newState)
{
    if (newState == _state)
        return;

    _state = newState;
    emit stateChanged(newState);
}

///
/// \brief ModbusClientPrivate::setError
/// \param errorText
/// \param error
///
void ModbusClientPrivate::setError(const QString& errorText, ModbusDevice::Error error)
{
    _error = error;
    _errorString = errorText;
    emit errorOccurred(error);
}

///
/// \brief ModbusClientPrivate::sendRequest
/// \param requestGroupId
/// \param request
/// \param serverAddress
/// \param unit
/// \return
///
ModbusReply *ModbusClientPrivate::sendRequest(int requestGroupId, const QModbusRequest &request, int serverAddress, const QModbusDataUnit* const unit)
{
    if (!isOpen() || state() != ModbusDevice::ConnectedState) {
        qCWarning(QT_MODBUS) << "(Client) Device is not connected";
        setError(QModbusClient::tr("Device not connected."), ModbusDevice::ConnectionError);
        return nullptr;
    }

    if (!request.isValid()) {
        qCWarning(QT_MODBUS) << "(Client) Refuse to send invalid request.";
        setError(QModbusClient::tr("Invalid Modbus request."), ModbusDevice::ProtocolError);
        return nullptr;
    }

    if (unit)
        return enqueueRequest(requestGroupId, request, serverAddress, *unit, ModbusReply::Common);
    return enqueueRequest(requestGroupId, request, serverAddress, QModbusDataUnit(), ModbusReply::Raw);
}

///
/// \brief ModbusClientPrivate::createReadRequest
/// \param data
/// \return
///
QModbusRequest ModbusClientPrivate::createReadRequest(const QModbusDataUnit& data) const
{
    if (!data.isValid())
        return QModbusRequest();

    switch (data.registerType()) {
    case QModbusDataUnit::Coils:
        return QModbusRequest(QModbusRequest::ReadCoils, quint16(data.startAddress()),
                              quint16(data.valueCount()));
    case QModbusDataUnit::DiscreteInputs:
        return QModbusRequest(QModbusRequest::ReadDiscreteInputs, quint16(data.startAddress()),
                              quint16(data.valueCount()));
    case QModbusDataUnit::InputRegisters:
        return QModbusRequest(QModbusRequest::ReadInputRegisters, quint16(data.startAddress()),
                              quint16(data.valueCount()));
    case QModbusDataUnit::HoldingRegisters:
        return QModbusRequest(QModbusRequest::ReadHoldingRegisters, quint16(data.startAddress()),
                              quint16(data.valueCount()));
    default:
        break;
    }

    return QModbusRequest();
}

///
/// \brief ModbusClientPrivate::createWriteRequest
/// \param data
/// \return
///
QModbusRequest ModbusClientPrivate::createWriteRequest(const QModbusDataUnit& data) const
{
    switch (data.registerType()) {
    case QModbusDataUnit::Coils: {
        if (data.valueCount() == 1) {
            return QModbusRequest(QModbusRequest::WriteSingleCoil, quint16(data.startAddress()),
                                  quint16((data.value(0) == 0u) ? Coil::Off : Coil::On));
        }

        quint8 byteCount = quint8(data.valueCount() / 8);
        if ((data.valueCount() % 8) != 0)
            byteCount += 1;

        qsizetype address = 0;
        QVector<quint8> bytes;
        for (quint8 i = 0; i < byteCount; ++i) {
            quint8 byte = 0;
            for (int currentBit = 0; currentBit < 8; ++currentBit)
                if (data.value(address++))
                    byte |= (1U << currentBit);
            bytes.append(byte);
        }

        return QModbusRequest(QModbusRequest::WriteMultipleCoils, quint16(data.startAddress()),
                              quint16(data.valueCount()), byteCount, bytes);
    }   break;

    case QModbusDataUnit::HoldingRegisters: {
        if (data.valueCount() == 1) {
            return QModbusRequest(QModbusRequest::WriteSingleRegister, quint16(data.startAddress()),
                                  data.value(0));
        }

        const quint8 byteCount = quint8(data.valueCount() * 2);
        return QModbusRequest(QModbusRequest::WriteMultipleRegisters, quint16(data.startAddress()),
                              quint16(data.valueCount()), byteCount, data.values());
    }   break;

    case QModbusDataUnit::DiscreteInputs:
    case QModbusDataUnit::InputRegisters:
    default:    // fall through on purpose
        break;
    }
    return QModbusRequest();
}

///
/// \brief ModbusClientPrivate::createRWRequest
/// \param read
/// \param write
/// \return
///
QModbusRequest ModbusClientPrivate::createRWRequest(const QModbusDataUnit& read, const QModbusDataUnit& write) const
{
    if ((read.registerType() != QModbusDataUnit::HoldingRegisters)
        || (write.registerType() != QModbusDataUnit::HoldingRegisters)) {
        return QModbusRequest();
    }

    const quint8 byteCount = quint8(write.valueCount() * 2);
    return QModbusRequest(QModbusRequest::ReadWriteMultipleRegisters, quint16(read.startAddress()),
                          quint16(read.valueCount()), quint16(write.startAddress()),
                          quint16(write.valueCount()), byteCount, write.values());
}

///
/// \brief ModbusClientPrivate::processQueueElement
/// \param pdu
/// \param serverAddress
/// \param element
///
void ModbusClientPrivate::processQueueElement(const QModbusResponse& pdu, int serverAddress, const QueueElement& element)
{
    if (element.reply.isNull())
        return;
    element.reply->setRawResult(pdu);

    if(serverAddress != element.reply->serverAddress()) {
        element.reply->setError(ModbusDevice::InvalidResponseError,
                                QModbusClient::tr("An invalid server address was received."));
        return;
    }

    if (pdu.isException()) {
        element.reply->setError(ModbusDevice::ProtocolError,
                                QModbusClient::tr("Modbus Exception Response."));
        return;
    }

    if (element.reply->type() == ModbusReply::Broadcast) {
        element.reply->setFinished(true);
        return;
    }

    QModbusDataUnit unit = element.unit;
    if (!processResponse(pdu, &unit)) {
        element.reply->setError(ModbusDevice::InvalidResponseError,
                                QModbusClient::tr("An invalid response has been received."));
        return;
    }

    element.reply->setResult(unit);
    element.reply->setFinished(true);
}

///
/// \brief ModbusClientPrivate::processResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    switch (response.functionCode()) {
    case QModbusRequest::ReadCoils:
        return processReadCoilsResponse(response, data);
    case QModbusRequest::ReadDiscreteInputs:
        return processReadDiscreteInputsResponse(response, data);
    case QModbusRequest::ReadHoldingRegisters:
        return processReadHoldingRegistersResponse(response, data);
    case QModbusRequest::ReadInputRegisters:
        return processReadInputRegistersResponse(response, data);
    case QModbusRequest::WriteSingleCoil:
        return processWriteSingleCoilResponse(response, data);
    case QModbusRequest::WriteSingleRegister:
        return processWriteSingleRegisterResponse(response, data);
    case QModbusRequest::ReadExceptionStatus:
    case QModbusRequest::Diagnostics:
    case QModbusRequest::GetCommEventCounter:
    case QModbusRequest::GetCommEventLog:
        return false;   // Return early, it's not a private response.
    case QModbusRequest::WriteMultipleCoils:
        return processWriteMultipleCoilsResponse(response, data);
    case QModbusRequest::WriteMultipleRegisters:
        return processWriteMultipleRegistersResponse(response, data);
    case QModbusRequest::ReportServerId:
    case QModbusRequest::ReadFileRecord:
    case QModbusRequest::WriteFileRecord:
    case QModbusRequest::MaskWriteRegister:
        return false;   // Return early, it's not a private response.
    case QModbusRequest::ReadWriteMultipleRegisters:
        return processReadWriteMultipleRegistersResponse(response, data);
    case QModbusRequest::ReadFifoQueue:
    case QModbusRequest::EncapsulatedInterfaceTransport:
        return false;   // Return early, it's not a private response.
    default:
        break;
    }
    return processPrivateResponse(response, data);
}

///
/// \brief ModbusClientPrivate::processPrivateResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processPrivateResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    Q_UNUSED(response);
    Q_UNUSED(data);
    return false;
}

///
/// \brief isValid
/// \param response
/// \param fc
/// \return
///
static bool isValid(const QModbusResponse& response, QModbusResponse::FunctionCode fc)
{
    if (!response.isValid())
        return false;
    if (response.isException())
        return false;
    if (response.functionCode() != fc)
        return false;
    return true;
}

///
/// \brief ModbusClientPrivate::processReadCoilsResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processReadCoilsResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::ReadCoils))
        return false;
    return collateBits(response, QModbusDataUnit::Coils, data);
}

///
/// \brief ModbusClientPrivate::processReadDiscreteInputsResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processReadDiscreteInputsResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::ReadDiscreteInputs))
        return false;
    return collateBits(response, QModbusDataUnit::DiscreteInputs, data);
}

///
/// \brief ModbusClientPrivate::collateBits
/// \param response
/// \param type
/// \param data
/// \return
///
bool ModbusClientPrivate::collateBits(const QModbusPdu& response, QModbusDataUnit::RegisterType type, QModbusDataUnit* data)
{
    if (response.dataSize() < QModbusResponse::minimumDataSize(response))
        return false;

    // byte count needs to match available bytes
    const quint8 byteCount = quint8(response.data().at(0));
    if ((response.dataSize() - 1) != byteCount)
        return false;

    if (data) {
        const int valueCount = byteCount *8;
        const QByteArray payload = response.data();

        qsizetype value = 0;
        QVector<quint16> values(valueCount);
        for (qsizetype i = 1; i < payload.size(); ++i) {
            const quint8 byte = quint8(payload.at(i));
            for (qint32 currentBit = 0; currentBit < 8 && value < valueCount; ++currentBit)
                values[value++] = (byte & (1U << currentBit) ? 1 : 0);
        }
        data->setValues(values);
        data->setRegisterType(type);
    }
    return true;
}

///
/// \brief ModbusClientPrivate::processReadHoldingRegistersResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processReadHoldingRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::ReadHoldingRegisters))
        return false;
    return collateBytes(response, QModbusDataUnit::HoldingRegisters, data);
}

///
/// \brief ModbusClientPrivate::processReadInputRegistersResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processReadInputRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::ReadInputRegisters))
        return false;
    return collateBytes(response, QModbusDataUnit::InputRegisters, data);
}

///
/// \brief ModbusClientPrivate::collateBytes
/// \param response
/// \param type
/// \param data
/// \return
///
bool ModbusClientPrivate::collateBytes(const QModbusPdu& response, QModbusDataUnit::RegisterType type, QModbusDataUnit* data)
{
    if (response.dataSize() < QModbusResponse::minimumDataSize(response))
        return false;

    // byte count needs to match available bytes
    const quint8 byteCount = quint8(response.data().at(0));
    if ((response.dataSize() - 1) != byteCount)
        return false;

    // byte count needs to be even to match full registers
    if (byteCount % 2 != 0)
        return false;

    if (data) {
        QDataStream stream(response.data().remove(0, 1));

        QVector<quint16> values;
        const quint8 itemCount = byteCount / 2;
        for (int i = 0; i < itemCount; i++) {
            quint16 tmp;
            stream >> tmp;
            values.append(tmp);
        }
        data->setValues(values);
        data->setRegisterType(type);
    }
    return true;
}

///
/// \brief ModbusClientPrivate::processWriteSingleCoilResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processWriteSingleCoilResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::WriteSingleCoil))
        return false;
    return collateSingleValue(response, QModbusDataUnit::Coils, data);
}

///
/// \brief ModbusClientPrivate::processWriteSingleRegisterResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processWriteSingleRegisterResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::WriteSingleRegister))
        return false;
    return collateSingleValue(response, QModbusDataUnit::HoldingRegisters, data);
}

///
/// \brief ModbusClientPrivate::collateSingleValue
/// \param response
/// \param type
/// \param data
/// \return
///
bool ModbusClientPrivate::collateSingleValue(const QModbusPdu& response, QModbusDataUnit::RegisterType type, QModbusDataUnit* data)
{
    if (response.dataSize() != QModbusResponse::minimumDataSize(response))
        return false;

    quint16 address = 0, value = 0xffff;
    response.decodeData(&address, &value);
    if ((type == QModbusDataUnit::Coils) && (value != Coil::Off) && (value != Coil::On))
        return false;

    if (data) {
        data->setRegisterType(type);
        data->setStartAddress(address);
        data->setValues(QVector<quint16> { value });
    }
    return true;
}

///
/// \brief ModbusClientPrivate::processWriteMultipleCoilsResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processWriteMultipleCoilsResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::WriteMultipleCoils))
        return false;
    return collateMultipleValues(response, QModbusDataUnit::Coils, data);
}

///
/// \brief ModbusClientPrivate::processWriteMultipleRegistersResponse
/// \param response
/// \param data
/// \return
///
bool ModbusClientPrivate::processWriteMultipleRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data)
{
    if (!isValid(response, QModbusResponse::WriteMultipleRegisters))
        return false;
    return collateMultipleValues(response, QModbusDataUnit::HoldingRegisters, data);
}

///
/// \brief ModbusClientPrivate::collateMultipleValues
/// \param response
/// \param type
/// \param data
/// \return
///
bool ModbusClientPrivate::collateMultipleValues(const QModbusPdu& response, QModbusDataUnit::RegisterType type, QModbusDataUnit* data)
{
    if (response.dataSize() != QModbusResponse::minimumDataSize(response))
        return false;

    quint16 address = 0, count = 0;
    response.decodeData(&address, &count);

    // number of registers to write is 1-123 per request
    if ((type == QModbusDataUnit::HoldingRegisters) && (count < 1 || count > 123))
        return false;

    if (data) {
        data->setValueCount(count);
        data->setRegisterType(type);
        data->setStartAddress(address);
    }
    return true;
}

///
/// \brief ModbusClientPrivate::processReadWriteMultipleRegistersResponse
/// \param resp
/// \param data
/// \return
///
bool ModbusClientPrivate::processReadWriteMultipleRegistersResponse(const QModbusResponse& resp, QModbusDataUnit* data)
{
    if (!isValid(resp, QModbusResponse::ReadWriteMultipleRegisters))
        return false;
    return collateBytes(resp, QModbusDataUnit::HoldingRegisters, data);
}
