#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include "modbusexception.h"
#include "modbusclient.h"

///
/// \brief ModbusClient::ModbusClient
/// \param parent
///
ModbusClient::ModbusClient(QObject *parent)
    : QObject{parent}
    ,_modbusClient(nullptr)
{
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
            _modbusClient->setProperty("ConnectionType", QVariant::fromValue(cd.Type));
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
            _modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
        }
        break;

        case ConnectionType::Serial:
            _modbusClient = new QModbusRtuSerialMaster(this);
            _modbusClient->setTimeout(cd.ModbusParams.SlaveResponseTimeOut);
            _modbusClient->setNumberOfRetries(cd.ModbusParams.NumberOfRetries);
            _modbusClient->setProperty("ConnectionType", QVariant::fromValue(cd.Type));
            _modbusClient->setProperty("DTRControl", cd.SerialParams.SetDTR);
            _modbusClient->setProperty("RTSControl", cd.SerialParams.SetRTS);
            _modbusClient->setProperty("ForceModbus15And16Func", cd.ModbusParams.ForceModbus15And16Func);
            ((QModbusRtuSerialMaster*)_modbusClient)->setInterFrameDelay(cd.ModbusParams.InterFrameDelay);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, cd.SerialParams.PortName);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, cd.SerialParams.Parity);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, cd.SerialParams.BaudRate);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, cd.SerialParams.WordLength);
            _modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, cd.SerialParams.StopBits);
            ((QSerialPort*)_modbusClient->device())->setFlowControl(cd.SerialParams.FlowControl);
        break;
    }

    if(_modbusClient)
    {
        connect(_modbusClient, &QModbusDevice::stateChanged, this, &ModbusClient::on_stateChanged);
        connect(_modbusClient, &QModbusDevice::errorOccurred, this, &ModbusClient::on_errorOccured);
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

    emit modbusRequest(requestId, request);
    if(auto reply = _modbusClient->sendReadRequest(dataUnit, server))
    {
        reply->setProperty("RequestId", requestId);
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
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, quint16 value)
{
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 1);
    data.setValue(0, value);

    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param inv
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, float value, bool inv)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
    v.asFloat = value;

    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 2);
    if(inv)
    {
        data.setValue(0, v.asUint16[1]);
        data.setValue(1, v.asUint16[0]);
    }
    else
    {
        data.setValue(0, v.asUint16[0]);
        data.setValue(1, v.asUint16[1]);
    }

    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param value
/// \param inv
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, double value, bool inv)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asDouble = value;

    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 4);
    if(inv)
    {
        data.setValue(0, v.asUint16[3]);
        data.setValue(1, v.asUint16[2]);
        data.setValue(2, v.asUint16[1]);
        data.setValue(3, v.asUint16[0]);
    }
    else
    {
        data.setValue(0, v.asUint16[0]);
        data.setValue(1, v.asUint16[1]);
        data.setValue(2, v.asUint16[2]);
        data.setValue(3, v.asUint16[3]);
    }

    return data;
}

///
/// \brief createHoldingRegistersDataUnit
/// \param newStartAddress
/// \param values
/// \return
///
QModbusDataUnit createHoldingRegistersDataUnit(int newStartAddress, const QVector<quint16>& values)
{
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, values.count());
    data.setValues(values);

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
                data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.value<QVector<quint16>>());
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
                    case DataDisplayMode::Decimal:
                    case DataDisplayMode::Integer:
                    case DataDisplayMode::Hex:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toUInt());
                    break;
                    case DataDisplayMode::FloatingPt:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toFloat(), false);
                    break;
                    case DataDisplayMode::SwappedFP:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toFloat(), true);
                    break;
                    case DataDisplayMode::DblFloat:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toDouble(), false);
                    break;
                    case DataDisplayMode::SwappedDbl:
                        data = createHoldingRegistersDataUnit(params.Address - 1, params.Value.toDouble(), true);
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
                errorDesc = "Coil Write Failure";
            break;

            case QModbusDataUnit::HoldingRegisters:
                errorDesc = "Register Write Failure";
            break;

            default:
            break;
        }

        emit modbusWriteError(errorDesc);
        return;
    }

    const bool useMultipleWriteFunc = _modbusClient->property("ForceModbus15And16Func").toBool();
    const auto request = createWriteRequest(data, useMultipleWriteFunc);
    if(!request.isValid()) return;

    emit modbusRequest(requestId, request);

    if(auto reply = _modbusClient->sendRawRequest(request, params.Node))
    {
        reply->setProperty("RequestId", requestId);
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
        emit modbusWriteError("Mask Write Register Failure");
        return;
    }

    QModbusRequest request(QModbusRequest::MaskWriteRegister, quint16(params.Address - 1), params.AndMask, params.OrMask);
    emit modbusRequest(requestId, request);

    if(auto reply = _modbusClient->sendRawRequest(request, params.Node))
    {
        reply->setProperty("RequestId", requestId);
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

    emit modbusReply(reply);

    const auto raw  = reply->rawResult();
    auto onError = [this, reply, raw](const QString& errorDesc)
    {
        if (reply->error() == QModbusDevice::ProtocolError)
            emit modbusWriteError(QString("%1. %2").arg(errorDesc, ModbusException(raw.exceptionCode())));
        else if(reply->error() != QModbusDevice::NoError)
            emit modbusWriteError(QString("%1. %2").arg(errorDesc, reply->errorString()));
    };

    switch(raw.functionCode())
    {
        case QModbusRequest::WriteSingleCoil:
        case QModbusRequest::WriteMultipleCoils:
            onError("Coil Write Failure");
        break;

        case QModbusRequest::WriteSingleRegister:
        case QModbusRequest::WriteMultipleRegisters:
            onError("Register Write Failure");
        break;

        case QModbusRequest::MaskWriteRegister:
            onError("Mask Register Write Failure");
        break;

    default:
        break;
    }

    reply->deleteLater();
}

///
/// \brief ModbusClient::on_errorOccured
/// \param error
///
void ModbusClient::on_errorOccured(QModbusDevice::Error error)
{
    if(error == QModbusDevice::ConnectionError)
    {
        emit modbusConnectionError(QString("Connection error. %1").arg(_modbusClient->errorString()));
    }
}

///
/// \brief ModbusClient::on_stateChanged
/// \param state
///
void ModbusClient::on_stateChanged(QModbusDevice::State state)
{
    const auto connType = _modbusClient->property("ConnectionType").value<ConnectionType>();
    if(connType == ConnectionType::Serial && state == QModbusDevice::ConnectedState)
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
}
