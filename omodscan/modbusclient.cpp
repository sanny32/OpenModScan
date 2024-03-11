#include <QModbusTcpClient>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QModbusRtuSerialMaster>
typedef QModbusRtuSerialMaster QModbusRtuSerialClient;
#else
#include <QModbusRtuSerialClient>
#endif

#include "formatutils.h"
#include "numericutils.h"
#include "modbusexception.h"
#include "modbusclient.h"

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
            _modbusClient = new QModbusTcpClient(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setNumberOfRetries(cd.ModbusParams.NumberOfRetries);
            _modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
        }
        break;

        case ConnectionType::Serial:
            _modbusClient = new QModbusRtuSerialClient(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setNumberOfRetries(cd.ModbusParams.NumberOfRetries);
            _modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
            _modbusClient->setProperty("DTRControl", cd.SerialParams.SetDTR);
            _modbusClient->setProperty("RTSControl", cd.SerialParams.SetRTS);
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            qobject_cast<QModbusRtuSerialClient*>(_modbusClient)->setInterFrameDelay(cd.ModbusParams.InterFrameDelay);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, cd.SerialParams.PortName);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, cd.SerialParams.Parity);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, cd.SerialParams.BaudRate);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, cd.SerialParams.WordLength);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, cd.SerialParams.StopBits);
            qobject_cast<QSerialPort*>(_modbusClient->device())->setFlowControl(cd.SerialParams.FlowControl);
        break;
    }

    if(_modbusClient)
    {
        _connectionType = cd.Type;
        connect(_modbusClient, &QModbusDevice::stateChanged, this, &ModbusClient::on_stateChanged);
        connect(_modbusClient, &QModbusDevice::errorOccurred, this, &ModbusClient::on_errorOccurred);
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
/// \param requestId
///
void ModbusClient::sendRawRequest(const QModbusRequest& request, int server, int requestId)
{
    if(_modbusClient == nullptr || state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    emit modbusRequest(requestId, server, ++_transactionId, request);
    if(auto reply = _modbusClient->sendRawRequest(request, server))
    {
        reply->setProperty("RequestId", requestId);
        reply->setProperty("TransactionId", _transactionId);
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &ModbusClient::on_readReply);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
    else
        emit modbusError(tr("Invalid Modbus Request"), requestId);
}

///
/// \brief ModbusClient::sendReadRequest
/// \param pointType
/// \param startAddress
/// \param valueCount
/// \param server
/// \param requestId
///
void ModbusClient::sendReadRequest(QModbusDataUnit::RegisterType pointType, int startAddress, quint16 valueCount, int server, int requestId)
{
    if(_modbusClient == nullptr || state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    const QModbusDataUnit dataUnit(pointType, startAddress, valueCount);
    const auto request = createReadRequest(dataUnit);
    if(!request.isValid()) return;

    emit modbusRequest(requestId, server, ++_transactionId, request);
    if(auto reply = _modbusClient->sendReadRequest(dataUnit, server))
    {
        reply->setProperty("RequestId", requestId);
        reply->setProperty("TransactionId", _transactionId);
        reply->setProperty("RequestData", QVariant::fromValue(dataUnit));
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &ModbusClient::on_readReply);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
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
/// \param requestId
///
void ModbusClient::writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params, int requestId)
{
    QModbusDataUnit data;
    if(params.Value.userType() == qMetaTypeId<QVector<quint16>>())
    {
        switch (pointType)
        {
            case QModbusDataUnit::Coils:
                data = createCoilsDataUnit(params.Address - 1, params.Value.value<QVector<quint16>>());
            break;

            case QModbusDataUnit::HoldingRegisters:
                data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.value<QVector<quint16>>(), params.Order);
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
                data = createCoilsDataUnit(params.Address - 1, params.Value.toBool());
            break;

            case QModbusDataUnit::HoldingRegisters:
                switch(params.DisplayMode)
                {
                    case DataDisplayMode::Binary:
                    case DataDisplayMode::UInt16:
                    case DataDisplayMode::Int16:
                    case DataDisplayMode::Hex:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toUInt(), params.Order);
                    break;
                    case DataDisplayMode::FloatingPt:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toFloat(), params.Order, false);
                    break;
                    case DataDisplayMode::SwappedFP:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toFloat(), params.Order, true);
                    break;
                    case DataDisplayMode::DblFloat:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toDouble(), params.Order, false);
                    break;
                    case DataDisplayMode::SwappedDbl:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toDouble(), params.Order, true);
                    break;

                    case DataDisplayMode::Int32:
                    case DataDisplayMode::UInt32:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toInt(), params.Order, false);
                    break;

                    case DataDisplayMode::SwappedInt32:
                    case DataDisplayMode::SwappedUInt32:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toInt(), params.Order, true);
                    break;
                }
            break;

            default:
            break;
        }
    }

    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
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

        emit modbusError(errorDesc, requestId);
        return;
    }

    const bool useMultipleWriteFunc = _modbusClient->property("ForceModbus15And16Func").toBool();
    const auto request = createWriteRequest(data, useMultipleWriteFunc);
    if(!request.isValid()) return;

    emit modbusRequest(requestId, params.Node, ++_transactionId, request);
    if(auto reply = _modbusClient->sendRawRequest(request, params.Node))
    {
        reply->setProperty("RequestId", requestId);
        reply->setProperty("TransactionId", _transactionId);
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &ModbusClient::on_writeReply);
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
/// \param requestId
///
void ModbusClient::maskWriteRegister(const ModbusMaskWriteParams& params, int requestId)
{
    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
    {
        emit modbusError(tr("Mask Write Register Failure"), requestId);
        return;
    }

    QModbusRequest request(QModbusRequest::MaskWriteRegister, quint16(params.Address - 1), params.AndMask, params.OrMask);
    emit modbusRequest(requestId, params.Node, ++_transactionId, request);

    if(auto reply = _modbusClient->sendRawRequest(request, params.Node))
    {
        reply->setProperty("RequestId", requestId);
        reply->setProperty("TransactionId", _transactionId);
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &ModbusClient::on_writeReply);
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
QModbusDevice::State ModbusClient::state() const
{
    if(_modbusClient)
        return _modbusClient->state();

    return QModbusDevice::UnconnectedState;
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
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) return;

    emit modbusReply(reply);
    reply->deleteLater();
}

///
/// \brief ModbusClient::on_writeReply
///
void ModbusClient::on_writeReply()
{
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) return;

    const auto raw  = reply->rawResult();

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    if(raw.functionCode() == QModbusRequest::MaskWriteRegister &&
       reply->error() == QModbusDevice::InvalidResponseError)
    {
        reply->blockSignals(true);
        reply->setError(QModbusDevice::NoError, QString());
        reply->blockSignals(false);
    }
#endif

    emit modbusReply(reply);

    auto onError = [this, reply, raw](const QString& errorDesc, int requestId)
    {
        if (reply->error() == QModbusDevice::ProtocolError)
        {
            ModbusException ex(raw.exceptionCode());
            emit modbusError(QString("%1. %2 (%3)").arg(errorDesc, ex, formatUInt8Value(DataDisplayMode::Hex, ex)), requestId);
        }
        else if(reply->error() != QModbusDevice::NoError)
            emit modbusError(QString("%1. %2").arg(errorDesc, reply->errorString()), requestId);
    };

    const int requestId = reply->property("RequestId").toInt();
    switch(raw.functionCode())
    {
        case QModbusRequest::WriteSingleCoil:
        case QModbusRequest::WriteMultipleCoils:
            onError(tr("Coil Write Failure"), requestId);
        break;

        case QModbusRequest::WriteSingleRegister:
        case QModbusRequest::WriteMultipleRegisters:
            onError(tr("Register Write Failure"), requestId);
        break;

        case QModbusRequest::MaskWriteRegister:
            onError(tr("Mask Register Write Failure"), requestId);
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
void ModbusClient::on_errorOccurred(QModbusDevice::Error error)
{
    if(error == QModbusDevice::ConnectionError)
    {
        emit modbusConnectionError(QString(tr("Connection error. %1")).arg(_modbusClient->errorString()));
    }
}

///
/// \brief ModbusClient::on_stateChanged
/// \param state
///
void ModbusClient::on_stateChanged(QModbusDevice::State state)
{
    const auto cd = _modbusClient->property("ConnectionDetails").value<ConnectionDetails>();
    switch(state)
    {
        case QModbusDevice::ConnectingState:
            emit modbusConnecting(cd);
        break;

        case QModbusDevice::ConnectedState:
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

            _transactionId = -1;
            emit modbusConnected(cd);
        }
        break;

        case QModbusDevice::UnconnectedState:
            emit modbusDisconnected(cd);
        break;

        default:
        break;
    }

}
