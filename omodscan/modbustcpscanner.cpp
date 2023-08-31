#include <QDateTime>
#include <QTcpSocket>
#include "modbustcpscanner.h"

///
/// \brief ModbusTcpScanner::ModbusTcpScanner
/// \param params
/// \param parent
///
ModbusTcpScanner::ModbusTcpScanner(const ScanParams& params, QObject *parent)
    : ModbusScanner{parent}
    ,_params(params)
{
    connect(this, &ModbusTcpScanner::scanDeviceFinished, this, &ModbusTcpScanner::on_scanDeviceFinished, Qt::QueuedConnection);
}

///
/// \brief ModbusTcpScanner::startScan
///
void ModbusTcpScanner::startScan()
{
    ModbusScanner::startScan();

    _processedSocketCount = 0;
    _itemsToScan.clear();

    for(auto&& cd : _params.ConnParams)
    {
        QTcpSocket* socket = new QTcpSocket(this);

        connect(socket, &QAbstractSocket::connected, this, [this, socket, cd]{
            processSocket(socket, cd);
        }, Qt::QueuedConnection);
        connect(socket, &QAbstractSocket::errorOccurred, this, [this, socket, cd](QAbstractSocket::SocketError){
            processSocket(socket, cd);
        }, Qt::QueuedConnection);

        socket->connectToHost(cd.TcpParams.IPAddress, cd.TcpParams.ServicePort, QIODevice::ReadOnly, QAbstractSocket::IPv4Protocol);
    }
}

///
/// \brief ModbusTcpScanner::internalScan
///
void ModbusTcpScanner::internalScan()
{
    std::sort(_itemsToScan.begin(), _itemsToScan.end(), [](const ConnectionDetails& cd1, const ConnectionDetails& cd2){
        return QHostAddress(cd1.TcpParams.IPAddress).toIPv4Address() < QHostAddress(cd2.TcpParams.IPAddress).toIPv4Address();
    });

    _iterator = _itemsToScan.cbegin();
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
/// \brief ModbusTcpScanner::processSocket
/// \param sck
/// \param cd
///
void ModbusTcpScanner::processSocket(QTcpSocket* sck, const ConnectionDetails& cd)
{
    if(sck->state() == QAbstractSocket::ConnectedState)
        _itemsToScan.push_back(cd);

    sck->deleteLater();

    if(++_processedSocketCount == _params.ConnParams.size())
        internalScan();
}

///
/// \brief ModbusTcpScanner::on_scanDeviceFinished
///
void ModbusTcpScanner::on_scanDeviceFinished(QPrivateSignal)
{
    if(++_iterator == _itemsToScan.cend())
        stopScan();
    else
        connectDevice(*_iterator);
}

///
/// \brief ModbusTcpScanner::connectDevice
/// \param cd
///
void ModbusTcpScanner::connectDevice(const ConnectionDetails& cd)
{
    auto modbusClient = new QModbusTcpClient(this);
    connect(modbusClient, &QModbusTcpClient::stateChanged, this, [this, modbusClient](QModbusDevice::State state){
        if(state == QModbusDevice::ConnectedState)
            sendRequest(modbusClient, _params.DeviceIds.from());
        //else if(state == QModbusDevice::UnconnectedState)
            //modbusClient->deleteLater();
        });
    modbusClient->setNumberOfRetries(0);
    modbusClient->setTimeout(_params.Timeout);
    modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
    modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
    modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
    modbusClient->connectDevice();
}

///
/// \brief ModbusTcpScanner::sendRequest
/// \param deviceId
///
void ModbusTcpScanner::sendRequest(QModbusTcpClient* client, int deviceId)
{
    if(!inProgress())
        return;

    if(deviceId > _params.DeviceIds.to())
    {
        client->disconnectDevice();
        emit scanDeviceFinished(QPrivateSignal());

        return;
    }

    const auto cd = client->property("ConnectionDetails").value<ConnectionDetails>();

    const double size = _itemsToScan.size();
    const double addrLen = (_params.DeviceIds.to() - _params.DeviceIds.from() + 1);
    const double total = size * addrLen;
    const double value = std::distance(_itemsToScan.cbegin(), _iterator) / size  + (deviceId - _params.DeviceIds.from() + 1) / total;
    emit progress(cd, deviceId, value * 100);

    client->setProperty("DeviceId", deviceId);
    if(auto reply = client->sendRawRequest(modbusRequest(), deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, client, deviceId, cd]()
                {
                    if(reply->error() != QModbusDevice::TimeoutError &&
                       reply->error() != QModbusDevice::ConnectionError &&
                       reply->error() != QModbusDevice::ReplyAbortedError)
                    {
                        emit found(cd, deviceId);
                    }
                    reply->deleteLater();

                    sendRequest(client, deviceId + 1);
                }, Qt::QueuedConnection);
        }
        else
        {
            delete reply; // broadcast replies return immediately
            sendRequest(client, deviceId + 1);
        }
    }
    else
    {
        sendRequest(client, deviceId + 1);
    }
}
