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

    for(auto it = _params.ConnParams.cbegin(); it != _params.ConnParams.end(); ++it)
    {
        const auto params = it->TcpParams;

        QTcpSocket* socket = new QTcpSocket(this);
        socket->setProperty("ConnectionDetails", QVariant::fromValue(*it));

        connect(socket, &QAbstractSocket::connected, this, [this, socket]{
            const auto cd = socket->property("ConnectionDetails").value<ConnectionDetails>();
            socket->disconnect();

            connectDevice(cd);
        });
        connect(socket, &QAbstractSocket::stateChanged, this, [socket](QAbstractSocket::SocketState state){
            if(state == QAbstractSocket::UnconnectedState) socket->deleteLater();
        });
        socket->connectToHost(params.IPAddress, params.ServicePort, QIODevice::ReadOnly, QAbstractSocket::IPv4Protocol);
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
    modbusClient->disconnectDevice();
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
        client->disconnect();
        return;
    }

    client->setProperty("DeviceId", deviceId);
    QModbusRequest req(QModbusPdu::ReportServerId);
    if(auto reply = client->sendRawRequest(req, deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, client, deviceId]()
                {
                    if(reply->error() != QModbusDevice::TimeoutError &&
                        reply->error() != QModbusDevice::ConnectionError &&
                        reply->error() != QModbusDevice::ReplyAbortedError)
                    {
                        const auto cd = client->property("ConnectionDetails").value<ConnectionDetails>();
                        emit found(cd, deviceId);
                    }
                    reply->deleteLater();

                    sendRequest(client, deviceId + 1);
                },
                Qt::QueuedConnection);
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
