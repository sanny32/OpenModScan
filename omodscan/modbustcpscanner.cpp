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
}

///
/// \brief ModbusTcpScanner::startScan
///
void ModbusTcpScanner::startScan()
{
    ModbusScanner::startScan();

    _proceesedScans = 0;
    for(auto&& cd : _params.ConnParams)
    {
        QTcpSocket* socket = new QTcpSocket(this);
        connect(socket, &QAbstractSocket::connected, this, [this, socket, cd]{
            socket->disconnectFromHost();
            connectDevice(cd);
            processScan(cd, _params.DeviceIds.from());
        });
        connect(socket, &QAbstractSocket::stateChanged, this, [this, socket, cd](QAbstractSocket::SocketState state){
            if(state == QAbstractSocket::UnconnectedState)
            {
                socket->deleteLater();
                processScan(cd, _params.DeviceIds.from());
            }
        });

        socket->connectToHost(cd.TcpParams.IPAddress, cd.TcpParams.ServicePort, QIODevice::ReadOnly, QAbstractSocket::IPv4Protocol);
    }
}

///
/// \brief ModbusTcpScanner::stopScan
///
void ModbusTcpScanner::stopScan()
{
    ModbusScanner::stopScan();
}

///
/// \brief ModbusTcpScanner::processScan
/// \param cd
/// \param deviceId
///
void ModbusTcpScanner::processScan(const ConnectionDetails& cd, int deviceId)
{
    const int value = ++_proceesedScans * 100.0 / _params.ConnParams.size();
    emit progress(cd, deviceId, value);

    if(value >= 100)
        stopScan();
}

///
/// \brief ModbusTcpScanner::connectDevice
/// \param cd
///
void ModbusTcpScanner::connectDevice(const ConnectionDetails& cd)
{
    auto modbusClient = new QModbusTcpClient(this);
    connect(modbusClient, &QModbusTcpClient::stateChanged, [this, modbusClient](QModbusDevice::State state){
        if(state == QModbusDevice::ConnectedState)
            sendRequest(modbusClient, _params.DeviceIds.from());
        else if(state == QModbusDevice::UnconnectedState)
            modbusClient->deleteLater();
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

    /*const double size = _params.ConnParams.size();
    const double addrLen = (_params.DeviceIds.to() - _params.DeviceIds.from() + 1);
    const double total = size * addrLen;
    const double value = std::distance(_params.ConnParams.cbegin(), _iterator) / size  + (deviceId - _params.DeviceIds.from() + 1) / total;
    emit progress(*_iterator, deviceId, value * 100);*/

    if(deviceId > _params.DeviceIds.to())
    {
        client->disconnectDevice();
        return;
    }

    client->setProperty("DeviceId", deviceId);
    if(auto reply = client->sendRawRequest(modbusRequest(), deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, client, deviceId]()
                {
                    if(reply->error() != QModbusDevice::TimeoutError &&
                       reply->error() != QModbusDevice::ConnectionError &&
                       reply->error() != QModbusDevice::ReplyAbortedError)
                    {
                        emit found(client->property("ConnectionDetails").value<ConnectionDetails>(), deviceId);
                    }
                    reply->deleteLater();

                    sendRequest(client, deviceId + 1);
                });
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
