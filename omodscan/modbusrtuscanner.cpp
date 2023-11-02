#include "modbusrtuscanner.h"

///
/// \brief ModbusRtuScanner::ModbusRtuScanner
/// \param params
///
ModbusRtuScanner::ModbusRtuScanner(const ScanParams& params, QObject* parent)
    : ModbusScanner(parent)
    ,_modbusClient(new QModbusRtuSerialClient(this))
    ,_params(params)
{
    connect(_modbusClient, &QModbusClient::stateChanged, this, &ModbusRtuScanner::on_stateChanged);
    connect(_modbusClient, &QModbusClient::errorOccurred, this, &ModbusRtuScanner::on_errorOccurred);

    auto serialPort = qobject_cast<QSerialPort*>(_modbusClient->device());
    QObject::connect(serialPort, &QSerialPort::readyRead, this,
    [this]()
    {
        emit found(*_iterator, _modbusClient->property("DeviceId").toInt(), true);
    });
}

///
/// \brief ModbusRtuScanner::startScan
///
void ModbusRtuScanner::startScan()
{
    ModbusScanner::startScan();

    _iterator = _params.ConnParams.cbegin();
    connectDevice(*_iterator);
}

///
/// \brief ModbusRtuScanner::stopScan
///
void ModbusRtuScanner::stopScan()
{
    _modbusClient->disconnectDevice();
    ModbusScanner::stopScan();
}

///
/// \brief ModbusRtuScanner::on_errorOccurred
/// \param error
///
void ModbusRtuScanner::on_errorOccurred(QModbusDevice::Error error)
{
    if(error == QModbusDevice::ConnectionError &&
        _modbusClient->state() == QModbusDevice::ConnectingState)
    {
        stopScan();
        emit errorOccurred(_modbusClient->errorString());
    }
}

///
/// \brief ModbusRtuScanner::on_stateChanged
/// \param state
///
void ModbusRtuScanner::on_stateChanged(QModbusDevice::State state)
{
    if(state == QModbusDevice::ConnectedState)
        sendRequest(_params.DeviceIds.from());
}

///
/// \brief ModbusRtuScanner::connectDevice
/// \param cd
///
void ModbusRtuScanner::connectDevice(const ConnectionDetails& cd)
{
    _modbusClient->disconnectDevice();
    _modbusClient->setNumberOfRetries(_params.RetryOnTimeout ? 1 : 0);
    _modbusClient->setTimeout(_params.Timeout);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, cd.SerialParams.PortName);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, cd.SerialParams.Parity);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, cd.SerialParams.BaudRate);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, cd.SerialParams.WordLength);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, cd.SerialParams.StopBits);
    _modbusClient->connectDevice();
}

///
/// \brief ModbusRtuScanner::sendRequest
/// \param deviceId
///
void ModbusRtuScanner::sendRequest(int deviceId)
{
    if(!inProgress())
        return;

    if(deviceId > _params.DeviceIds.to())
    {
        _iterator++;

        if(_iterator != _params.ConnParams.end())
            connectDevice(*_iterator);
        else
            stopScan();

        return;
    }

    const double size = _params.ConnParams.size();
    const double addrLen = (_params.DeviceIds.to() - _params.DeviceIds.from() + 1);
    const double total = size * addrLen;
    const double value = std::distance(_params.ConnParams.cbegin(), _iterator) / size  + (deviceId - _params.DeviceIds.from() + 1) / total;
    emit progress(*_iterator, deviceId, value * 100);

    _modbusClient->setProperty("DeviceId", deviceId);
    if(auto reply = _modbusClient->sendRawRequest(_params.Request, deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, deviceId]()
                {
                    if(reply->error() != QModbusDevice::TimeoutError &&
                        reply->error() != QModbusDevice::ConnectionError &&
                        reply->error() != QModbusDevice::ReplyAbortedError)
                    {
                        emit found(*_iterator, deviceId, false);
                    }
                    reply->deleteLater();

                    sendRequest(deviceId + 1);
                },
                Qt::QueuedConnection);
        }
        else
        {
            delete reply; // broadcast replies return immediately
            sendRequest(deviceId + 1);
        }
    }
    else
    {
        sendRequest(deviceId + 1);
    }
}
