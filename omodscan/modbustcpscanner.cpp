#include "modbustcpscanner.h"

///
/// \brief ModbusTcpScanner::ModbusTcpScanner
/// \param params
/// \param parent
///
ModbusTcpScanner::ModbusTcpScanner(const ScanParams& params, QObject *parent)
    : ModbusScanner{parent}
    ,_modbusClient(new QModbusTcpClient(this))
    ,_params(params)
{
    connect(_modbusClient, &QModbusClient::stateChanged, this, &ModbusTcpScanner::on_stateChanged);
    connect(_modbusClient, &QModbusClient::errorOccurred, this, &ModbusTcpScanner::on_errorOccurred);
}

///
/// \brief ModbusTcpScanner::startScan
///
void ModbusTcpScanner::startScan()
{
    ModbusScanner::startScan();

    _iterator = _params.ConnParams.cbegin();
    connectDevice(*_iterator);
}

///
/// \brief ModbusTcpScanner::stopScan
///
void ModbusTcpScanner::stopScan()
{
    ModbusScanner::stopScan();
}

///
/// \brief ModbusTcpScanner::on_errorOccurred
/// \param error
///
void ModbusTcpScanner::on_errorOccurred(QModbusDevice::Error error)
{

}

///
/// \brief ModbusTcpScanner::on_stateChanged
/// \param state
///
void ModbusTcpScanner::on_stateChanged(QModbusDevice::State state)
{
    if(state == QModbusDevice::ConnectedState)
        sendRequest(_params.DeviceIds.from());
}

///
/// \brief ModbusTcpScanner::connectDevice
/// \param cd
///
void ModbusTcpScanner::connectDevice(const ConnectionDetails& cd)
{
    _modbusClient->disconnectDevice();
    _modbusClient->setNumberOfRetries(0);
    _modbusClient->setTimeout(_params.Timeout);
    _modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
    _modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
    _modbusClient->connectDevice();
}

///
/// \brief ModbusTcpScanner::sendRequest
/// \param deviceId
///
void ModbusTcpScanner::sendRequest(int deviceId)
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
    QModbusRequest req(QModbusPdu::ReportServerId);
    if(auto reply = _modbusClient->sendRawRequest(req, deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, deviceId]()
                {
                    if(reply->error() != QModbusDevice::TimeoutError &&
                        reply->error() != QModbusDevice::ConnectionError &&
                        reply->error() != QModbusDevice::ReplyAbortedError)
                    {
                        emit found(*_iterator, deviceId);
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
