#include <QDateTime>
#include <QTimer>
#include <QTcpSocket>
#include "modbustcpclient.h"
#include "modbustcpscanner.h"

///
/// \brief ModbusTcpScanner::ModbusTcpScanner
/// \param params
/// \param parent
///
ModbusTcpScanner::ModbusTcpScanner(const ScanParams& params, QObject *parent)
    : ModbusScanner{parent}
    ,_params(params)
    ,_processedSocketCount(0)
{
    connect(this, &ModbusTcpScanner::scanNext, this, &ModbusTcpScanner::on_scanNext);
}

///
/// \brief ModbusTcpScanner::startScan
///
void ModbusTcpScanner::startScan()
{
    ModbusScanner::startScan();

    _connParams.clear();
    _processedSocketCount = 0;

    for(auto&& cd : _params.ConnParams)
    {
        QTcpSocket* socket = new QTcpSocket(this);
        auto processed = QSharedPointer<bool>::create(false);

        auto processOnce = [this, socket, cd, processed]{
            if(!*processed) { *processed = true; processSocket(socket, cd); }
        };

        // After connect: send a probe byte so that the firewall/router
        // will RST phantom connections (no real host behind the IP).
        connect(socket, &QAbstractSocket::connected, this, [socket]{
            socket->write("\x00", 1);
        }, Qt::QueuedConnection);

        // RST or connection refused → immediately rejected
        connect(socket, &QAbstractSocket::disconnected, this, processOnce, Qt::QueuedConnection);
        connect(socket, &QAbstractSocket::errorOccurred, this, [processOnce](QAbstractSocket::SocketError){
            processOnce();
        }, Qt::QueuedConnection);

        // If still connected after 200 ms → treat as a real host
        QTimer::singleShot(200, this, [processOnce]{ processOnce(); });

        socket->connectToHost(cd.TcpParams.IPAddress, cd.TcpParams.ServicePort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
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
/// \brief ModbusTcpScanner::processSocket
/// \param sck
/// \param cd
///
void ModbusTcpScanner::processSocket(QTcpSocket* sck, const ConnectionDetails& cd)
{
    if(!inProgress())
        return;

    _processedSocketCount++;

    if(sck->state() == QAbstractSocket::ConnectedState)
        _connParams.push_back(cd);
    else
    {
        const double value = (_processedSocketCount - _connParams.size()) * 100. / _params.ConnParams.size();
        emit progress(cd, _params.DeviceIds.from(), value);
    }

    sck->deleteLater();

    if(_processedSocketCount == _params.ConnParams.size())
    {
        std::sort(_connParams.begin(), _connParams.end(), [](const ConnectionDetails& cd1, const ConnectionDetails& cd2){
            return QHostAddress(cd1.TcpParams.IPAddress).toIPv4Address() < QHostAddress(cd2.TcpParams.IPAddress).toIPv4Address();
        });

        emit scanNext(QPrivateSignal());
    }
}

///
/// \brief ModbusTcpScanner::on_scanNext
///
void ModbusTcpScanner::on_scanNext(QPrivateSignal)
{
    if(!inProgress())
        return;

    if(_connParams.isEmpty())
        stopScan();
    else
        connectDevice(_connParams.dequeue());
}

///
/// \brief ModbusTcpScanner::createClient
/// \return
///
ModbusClientPrivate* ModbusTcpScanner::createClient()
{
    return new ModbusTcpClient(this);
}

///
/// \brief ModbusTcpScanner::connectDevice
/// \param cd
///
void ModbusTcpScanner::connectDevice(const ConnectionDetails& cd)
{
    auto modbusClient = createClient();
    connect(modbusClient, &ModbusClientPrivate::stateChanged, this, [this, modbusClient](ModbusDevice::State state){
        if(state == ModbusDevice::ConnectedState)
            sendRequest(modbusClient, _params.DeviceIds.from());
        });
    modbusClient->disconnectDevice();
    modbusClient->setNumberOfRetries(_params.RetryOnTimeout ? 1 : 0);
    modbusClient->setTimeout(_params.Timeout);
    modbusClient->setProperty("ConnectionDetails", QVariant::fromValue(cd));
    modbusClient->setConnectionParameter(ModbusDevice::NetworkAddressParameter, cd.TcpParams.IPAddress);
    modbusClient->setConnectionParameter(ModbusDevice::NetworkPortParameter, cd.TcpParams.ServicePort);
    modbusClient->connectDevice();
}

///
/// \brief ModbusTcpScanner::sendRequest
/// \param deviceId
///
void ModbusTcpScanner::sendRequest(ModbusClientPrivate* client, int deviceId)
{
    if(!inProgress())
        return;

    if(deviceId > _params.DeviceIds.to())
    {
        client->deleteLater();
        emit scanNext(QPrivateSignal());

        return;
    }

    const auto cd = client->property("ConnectionDetails").value<ConnectionDetails>();
    const double curr = (deviceId - _params.DeviceIds.from() + 1) / (double)(_params.DeviceIds.to() - _params.DeviceIds.from() + 1);
    const double total = (_processedSocketCount - _connParams.size() - 1) / (double)_params.ConnParams.size();
    const double value = total + (1 - total) * curr / (_connParams.size() + 1);
    emit progress(cd, deviceId, value * 100);

    if(auto reply = client->sendRawRequest(_params.Request, deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &ModbusReply::finished, this, [this, client, reply, deviceId, cd]()
                {
                    const auto error = reply->error();
                    if(error != ModbusDevice::TimeoutError &&
                       error != ModbusDevice::ConnectionError &&
                       error != ModbusDevice::ReplyAbortedError)
                    {
                        if(error == ModbusDevice::ProtocolError)
                        {
                            switch(reply->rawResult().exceptionCode())
                            {
                                case QModbusPdu::GatewayPathUnavailable:
                                case QModbusPdu::GatewayTargetDeviceFailedToRespond:
                                break;

                                default:
                                    emit found(cd, deviceId, false);
                                break;
                            }
                        }
                        else
                        {
                            emit found(cd, deviceId, false);
                        }
                    }
                    reply->deleteLater();

                    if(error == ModbusDevice::TimeoutError)
                        sendRequest(client, deviceId + 1);
                    else
                        QTimer::singleShot(_params.Timeout, [this, client, deviceId] { sendRequest(client, deviceId + 1); });
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
