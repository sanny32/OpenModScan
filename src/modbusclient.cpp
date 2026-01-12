#include <QTimer>
#include <QEventLoop>
#include "formatutils.h"
#include "numericutils.h"
#include "modbusexception.h"
#include "modbusclient.h"
#include "modbustcpclient.h"
#include "modbusrtuclient.h"

///
/// \brief ModbusClient::ModbusClient
/// \param parent
///
ModbusClient::ModbusClient(QObject *parent)
    : QObject{parent}
    ,_modbusClient(nullptr)
    ,_connectionType(ConnectionType::Serial)
{
}

///
/// \brief ModbusClient::~ModbusClient
///
ModbusClient::~ModbusClient()
{
    if(_modbusClient)
        delete _modbusClient;
}

///
/// \brief ModbusClient::connectDevice
/// \param cd
///
void ModbusClient::connectDevice(const ConnectionDetails& cd)
{
    if(_modbusClient != nullptr)
    {
        delete _modbusClient;
        _modbusClient = nullptr;
    }

    switch(cd.Type)
    {
        case ConnectionType::Tcp:
        {
            _modbusClient = new ModbusTcpClient(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setNumberOfRetries(cd.ModbusParams.NumberOfRetries);
            _modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            _modbusClient->setConnectionParameter(ModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
            _modbusClient->setConnectionParameter(ModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
        }
        break;

        case ConnectionType::Serial:
            _modbusClient = new ModbusRtuClient(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setNumberOfRetries(cd.ModbusParams.NumberOfRetries);
            _modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
            _modbusClient->setProperty("DTRControl", cd.SerialParams.SetDTR);
            _modbusClient->setProperty("RTSControl", cd.SerialParams.SetRTS);
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            qobject_cast<ModbusRtuClient*>(_modbusClient)->setInterFrameDelay(cd.ModbusParams.InterFrameDelay);
            _modbusClient->setConnectionParameter(ModbusDevice::SerialPortNameParameter, cd.SerialParams.PortName);
            _modbusClient->setConnectionParameter(ModbusDevice::SerialParityParameter, cd.SerialParams.Parity);
            _modbusClient->setConnectionParameter(ModbusDevice::SerialBaudRateParameter, cd.SerialParams.BaudRate);
            _modbusClient->setConnectionParameter(ModbusDevice::SerialDataBitsParameter, cd.SerialParams.WordLength);
            _modbusClient->setConnectionParameter(ModbusDevice::SerialStopBitsParameter, cd.SerialParams.StopBits);
            qobject_cast<QSerialPort*>(_modbusClient->device())->setFlowControl(cd.SerialParams.FlowControl);
        break;
    }

    if(_modbusClient)
    {
        _connectionType = cd.Type;
        connect(_modbusClient, &ModbusClientPrivate::stateChanged, this, &ModbusClient::on_stateChanged);
        connect(_modbusClient, &ModbusClientPrivate::errorOccurred, this, &ModbusClient::on_errorOccurred);
        connect(_modbusClient, &ModbusClientPrivate::modbusRequest, this, &ModbusClient::modbusRequest);
        connect(_modbusClient, &ModbusClientPrivate::modbusResponse, this, &ModbusClient::modbusResponse);
        _modbusClient->connectDevice();
    }
}

///
/// \brief ModbusClient::disconnectDevice
///
void ModbusClient::disconnectDevice()
{
    if(_modbusClient)
        _modbusClient->disconnectDevice();
}

///
/// \brief createReadRequest
/// \param data
/// \return
///
QModbusRequest createReadRequest(const QModbusDataUnit& data)
{
    switch (data.registerType())
    {
        case QModbusDataUnit::Coils:
            return QModbusRequest(QModbusRequest::ReadCoils, quint16(data.startAddress()), quint16(data.valueCount()));
        case QModbusDataUnit::DiscreteInputs:
            return QModbusRequest(QModbusRequest::ReadDiscreteInputs, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        case QModbusDataUnit::InputRegisters:
            return QModbusRequest(QModbusRequest::ReadInputRegisters, quint16(data.startAddress()), quint16(data.valueCount()));
        case QModbusDataUnit::HoldingRegisters:
            return QModbusRequest(QModbusRequest::ReadHoldingRegisters, quint16(data.startAddress()), quint16(data.valueCount()));
        default:
        break;
    }

    return QModbusRequest();
}

///
/// \brief ModbusClient::sendRawRequest
/// \param request
/// \param server
/// \param requestGroupId
///
void ModbusClient::sendRawRequest(const QModbusRequest& request, int server, int requestGroupId)
{
    if(_modbusClient == nullptr || state() != ModbusDevice::ConnectedState)
    {
        return;
    }

    if(auto reply = _modbusClient->sendRawRequest(request, server, requestGroupId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &ModbusReply::finished, this, &ModbusClient::on_readReply);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
    else
        emit modbusError(tr("Invalid Modbus Request"), requestGroupId);
}

///
/// \brief ModbusClient::sendReadRequest
/// \param pointType
/// \param startAddress
/// \param valueCount
/// \param server
/// \param requestGroupId
///
void ModbusClient::sendReadRequest(QModbusDataUnit::RegisterType pointType, int startAddress, quint16 valueCount, int server, int requestGroupId)
{
    if(_modbusClient == nullptr || state() != ModbusDevice::ConnectedState)
    {
        return;
    }

    const QModbusDataUnit dataUnit(pointType, startAddress, valueCount);
    const auto request = createReadRequest(dataUnit);
    if(!request.isValid()) return;

    if(auto reply = _modbusClient->sendReadRequest(dataUnit, server, requestGroupId))
    {
        reply->setProperty("RequestData", QVariant::fromValue(dataUnit));
        if (!reply->isFinished())
        {
            connect(reply, &ModbusReply::finished, this, &ModbusClient::on_readReply);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
}

///
/// \brief ModbusClient::syncReadRegister
/// \param pointType
/// \param address
/// \param server
/// \return
///
quint16 ModbusClient::syncReadRegister(QModbusDataUnit::RegisterType pointType, int address, int server)
{
    if(_modbusClient == nullptr || state() != ModbusDevice::ConnectedState)
    {
        return 0;
    }

    const QModbusDataUnit dataUnit(pointType, address, 1);
    const auto request = createReadRequest(dataUnit);
    if(!request.isValid()) return 0;

    auto reply = _modbusClient->sendReadRequest(dataUnit, server);
    if(!reply) return 0;

    QEventLoop loop;
    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &ModbusReply::finished, &loop, &QEventLoop::quit);

    timer.setSingleShot(true);
    timer.start(timeout());

    loop.exec();
    if (!timer.isActive())
    {
        reply->deleteLater();
        return 0;
    }

    const auto result = reply->result();
    reply->deleteLater();

    return result.value(0);
}

///
/// \brief createWriteRequest
/// \param data
/// \param useMultipleWriteFunc
/// \return
///
QModbusRequest createWriteRequest(const QModbusDataUnit& data, bool useMultipleWriteFunc)
{
    switch (data.registerType())
    {
        case QModbusDataUnit::Coils:
        {
            if (data.valueCount() == 1 && !useMultipleWriteFunc)
            {
                return QModbusRequest(QModbusRequest::WriteSingleCoil, quint16(data.startAddress()),
                                      quint16((data.value(0) == 0u) ? 0x0000 : 0xFF00));
            }

            quint8 byteCount = data.valueCount() / 8;
            if ((data.valueCount() % 8) != 0)
                byteCount += 1;

            quint8 address = 0;
            QVector<quint8> bytes;
            for (quint8 i = 0; i < byteCount; ++i)
            {
                quint8 byte = 0;
                for (int currentBit = 0; currentBit < 8; ++currentBit)
                    if (data.value(address++))
                        byte |= (1U << currentBit);
                bytes.append(byte);
            }

            return QModbusRequest(QModbusRequest::WriteMultipleCoils, quint16(data.startAddress()),
                                  quint16(data.valueCount()), byteCount, bytes);
        }
        break;
        case QModbusDataUnit::HoldingRegisters:
        {
            if (data.valueCount() == 1 && !useMultipleWriteFunc)
            {
                return QModbusRequest(QModbusRequest::WriteSingleRegister, quint16(data.startAddress()),
                                        data.value(0));
            }

            const quint8 byteCount = data.valueCount() * 2;
            return QModbusRequest(QModbusRequest::WriteMultipleRegisters, quint16(data.startAddress()),
                                     quint16(data.valueCount()), byteCount, data.values());
        }
        break;
        default:
        break;
    }

    return QModbusRequest();
}

///
/// \brief createCoilsDataUnit
/// \param newStartAddress
/// \param value
/// \return
///
QModbusDataUnit createCoilsDataUnit(int newStartAddress, bool value)
{
    auto data = QModbusDataUnit(QModbusDataUnit::Coils, newStartAddress, 1);
    data.setValue(0, value);

    return data;
}

///
/// \brief createCoilsDataUnit
/// \param newStartAddress
/// \param values
/// \return
///
QModbusDataUnit createCoilsDataUnit(int newStartAddress, const QVector<quint16>& values)
{
    auto data = QModbusDataUnit(QModbusDataUnit::Coils, newStartAddress, values.count());
    data.setValues(values);

    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param order
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, quint16 value, ByteOrder order)
{
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 1);
    data.setValue(0, toByteOrderValue(value, order));

    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param order
/// \param swapped
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, float value, ByteOrder order, bool swapped)
{
    QVector<quint16> values(2);
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 2);

    if(swapped)
        breakFloat(value, values[1], values[0], order);
    else
        breakFloat(value, values[0], values[1], order);

    data.setValues(values);
    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param order
/// \param swapped
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, qint32 value, ByteOrder order, bool swapped)
{
    QVector<quint16> values(2);
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 2);

    if(swapped)
        breakInt32(value, values[1], values[0], order);
    else
        breakInt32(value, values[0], values[1], order);

    data.setValues(values);
    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param order
/// \param swapped
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, qint64 value, ByteOrder order, bool swapped)
{
    QVector<quint16> values(4);
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 4);

    if(swapped)
        breakInt64(value, values[3], values[2], values[1], values[0], order);
    else
        breakInt64(value, values[0], values[1], values[2], values[3], order);

    data.setValues(values);
    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param order
/// \param swapped
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, double value, ByteOrder order, bool swapped)
{
    QVector<quint16> values(4);
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 4);

    if(swapped)
        breakDouble(value, values[3], values[2], values[1], values[0], order);
    else
        breakDouble(value, values[0], values[1], values[2], values[3], order);

    data.setValues(values);
    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param values
/// \param order
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, const QVector<quint16>& values, ByteOrder order)
{
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, values.count());

    if(!values.isEmpty())
    {
        QVector<quint16> vv(values.size());
        for(int i = 0; i < vv.size(); i++)
            vv[i] = toByteOrderValue(values[i], order);

        data.setValues(values);
    }

    return data;
}

///
/// \brief ModbusClient::writeRegister
/// \param pointType
/// \param params
/// \param requestGroupId
///
void ModbusClient::writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params, int requestGroupId)
{
    QModbusDataUnit data;
    const auto addr = params.ZeroBasedAddress ? params.Address : params.Address - 1;

    if(params.Value.userType() == qMetaTypeId<QVector<quint16>>())
    {
        switch (pointType)
        {
            case QModbusDataUnit::Coils:
                data = createCoilsDataUnit(addr, params.Value.value<QVector<quint16>>());
            break;

            case QModbusDataUnit::HoldingRegisters:
                data = createHoldingRegistersDataUnit(addr, params.Value.value<QVector<quint16>>(), params.Order);
            break;

            default:
            break;
        }
    }
    else
    {
        switch (pointType)
        {
            case QModbusDataUnit::Coils:
                data = createCoilsDataUnit(addr, params.Value.toBool());
            break;

            case QModbusDataUnit::HoldingRegisters:
                switch(params.DisplayMode)
                {
                    case DataDisplayMode::Binary:
                    case DataDisplayMode::UInt16:
                    case DataDisplayMode::Int16:
                    case DataDisplayMode::Hex:
                    case DataDisplayMode::Ansi:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toUInt(), params.Order);
                    break;
                    case DataDisplayMode::FloatingPt:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toFloat(), params.Order, false);
                    break;
                    case DataDisplayMode::SwappedFP:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toFloat(), params.Order, true);
                    break;
                    case DataDisplayMode::DblFloat:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toDouble(), params.Order, false);
                    break;
                    case DataDisplayMode::SwappedDbl:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toDouble(), params.Order, true);
                    break;

                    case DataDisplayMode::Int32:
                    case DataDisplayMode::UInt32:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toInt(), params.Order, false);
                    break;

                    case DataDisplayMode::SwappedInt32:
                    case DataDisplayMode::SwappedUInt32:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toInt(), params.Order, true);
                    break;

                    case DataDisplayMode::Int64:
                    case DataDisplayMode::UInt64:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toLongLong(), params.Order, false);
                    break;

                    case DataDisplayMode::SwappedInt64:
                    case DataDisplayMode::SwappedUInt64:
                        data = createHoldingRegistersDataUnit(addr, params.Value.toLongLong(), params.Order, true);
                    break;
                }
            break;

            default:
            break;
        }
    }

    if(_modbusClient == nullptr ||
       _modbusClient->state() != ModbusDevice::ConnectedState)
    {
        QString errorDesc;
        switch(pointType)
        {
            case QModbusDataUnit::Coils:
                errorDesc = tr("Coil Write Failure");
            break;

            case QModbusDataUnit::HoldingRegisters:
                errorDesc = tr("Register Write Failure");
            break;

            default:
            break;
        }

        emit modbusError(errorDesc, requestGroupId);
        return;
    }

    const auto request = createWriteRequest(data, isForcedModbus15And16Func());
    if(!request.isValid()) return;

    if(auto reply = _modbusClient->sendRawRequest(request, params.DeviceId, requestGroupId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &ModbusReply::finished, this, &ModbusClient::on_writeReply);
        }
        else
        {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    }
}

///
/// \brief ModbusClient::maskWriteRegister
/// \param params
/// \param requestGroupId
///
void ModbusClient::maskWriteRegister(const ModbusMaskWriteParams& params, int requestGroupId)
{
    if(_modbusClient == nullptr ||
       _modbusClient->state() != ModbusDevice::ConnectedState)
    {
        emit modbusError(tr("Mask Write Register Failure"), requestGroupId);
        return;
    }

    const auto addr = params.ZeroBasedAddress ? params.Address : params.Address - 1;
    QModbusRequest request(QModbusRequest::MaskWriteRegister, quint16(addr), params.AndMask, params.OrMask);

    if(auto reply = _modbusClient->sendRawRequest(request, params.DeviceId, requestGroupId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &ModbusReply::finished, this, &ModbusClient::on_writeReply);
        }
        else
        {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    }
}

///
/// \brief ModbusClient::isValid
/// \return
///
bool ModbusClient::isValid() const
{
    return _modbusClient != nullptr;
}

///
/// \brief ModbusClient::state
/// \return
///
ModbusDevice::State ModbusClient::state() const
{
    if(_modbusClient)
        return _modbusClient->state();

    return ModbusDevice::UnconnectedState;
}

///
/// \brief ModbusClient::isForcedModbus15And16Func
/// \return
///
bool ModbusClient::isForcedModbus15And16Func() const
{
    return _modbusClient->property("ForceModbus15And16Func").toBool();
}

///
/// \brief ModbusClient::timeout
/// \return
///
int ModbusClient::timeout() const
{
    if(_modbusClient)
        return _modbusClient->timeout();

    return 0;
}

///
/// \brief ModbusClient::setTimeout
/// \param newTimeout
///
void ModbusClient::setTimeout(int newTimeout)
{
    if(_modbusClient)
        _modbusClient->setTimeout(newTimeout);
}

///
/// \brief ModbusClient::numberOfRetries
/// \return
///
uint ModbusClient::numberOfRetries() const
{
    if(_modbusClient)
        return _modbusClient->numberOfRetries();

    return 0;
}

///
/// \brief ModbusClient::setNumberOfRetries
/// \param number
///
void ModbusClient::setNumberOfRetries(uint number)
{
    if(_modbusClient)
        _modbusClient->setNumberOfRetries(number);
}

///
/// \brief ModbusClient::on_readReply
///
void ModbusClient::on_readReply()
{
    auto reply = qobject_cast<ModbusReply*>(sender());
    if (!reply) return;

    emit modbusReply(reply);
    reply->deleteLater();
}

///
/// \brief ModbusClient::on_writeReply
///
void ModbusClient::on_writeReply()
{
    auto reply = qobject_cast<ModbusReply*>(sender());
    if (!reply) return;

    const auto raw  = reply->rawResult();

    if(raw.functionCode() == QModbusRequest::MaskWriteRegister &&
       reply->error() == ModbusDevice::InvalidResponseError)
    {
        reply->blockSignals(true);
        reply->setError(ModbusDevice::NoError, QString());
        reply->blockSignals(false);
    }

    emit modbusReply(reply);

    auto onError = [this, reply, raw](const QString& errorDesc, int requestGroupId)
    {
        if (reply->error() == ModbusDevice::ProtocolError)
        {
            ModbusException ex(raw.exceptionCode());
            emit modbusError(QString("%1. %2 (%3)").arg(errorDesc, ex, formatUInt8Value(DataDisplayMode::Hex, true, ex)), requestGroupId);
        }
        else if(reply->error() != ModbusDevice::NoError)
            emit modbusError(QString("%1. %2").arg(errorDesc, reply->errorString()), requestGroupId);
    };

    const int requestGroupId = reply->requestGroupId();
    switch(raw.functionCode())
    {
        case QModbusRequest::WriteSingleCoil:
        case QModbusRequest::WriteMultipleCoils:
            onError(tr("Coil Write Failure"), requestGroupId);
        break;

        case QModbusRequest::WriteSingleRegister:
        case QModbusRequest::WriteMultipleRegisters:
            onError(tr("Register Write Failure"), requestGroupId);
        break;

        case QModbusRequest::MaskWriteRegister:
            onError(tr("Mask Register Write Failure"), requestGroupId);
        break;

    default:
        break;
    }

    reply->deleteLater();
}

///
/// \brief ModbusClient::on_errorOccurred
/// \param error
///
void ModbusClient::on_errorOccurred(ModbusDevice::Error error)
{
    if(error == ModbusDevice::ConnectionError)
    {
        emit modbusConnectionError(QString(tr("Connection error. %1")).arg(_modbusClient->errorString()));
    }
}

///
/// \brief ModbusClient::on_stateChanged
/// \param state
///
void ModbusClient::on_stateChanged(ModbusDevice::State state)
{
    const auto cd = _modbusClient->property("ConnectionDetails").value<ConnectionDetails>();
    switch(state)
    {
        case ModbusDevice::ConnectingState:
            emit modbusConnecting(cd);
        break;

        case ModbusDevice::ConnectedState:
        {
            if(cd.Type == ConnectionType::Serial)
            {
                auto port = (QSerialPort*)_modbusClient->device();

                const bool setDTR = _modbusClient->property("DTRControl").toBool();
                port->setDataTerminalReady(setDTR);

                if(port->flowControl() != QSerialPort::HardwareControl)
                {
                    const bool setRTS = _modbusClient->property("RTSControl").toBool();
                    port->setRequestToSend(setRTS);
                }
            }

            emit modbusConnected(cd);
        }
        break;

        case ModbusDevice::UnconnectedState:
            emit modbusDisconnected(cd);
        break;

        default:
        break;
    }

}
