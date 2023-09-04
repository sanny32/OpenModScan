#ifndef MODBUSTCPSCANNER_H
#define MODBUSTCPSCANNER_H

#include <QStack>
#include <QTcpSocket>
#include <QModbusTcpClient>
#include "modbusscanner.h"

///
/// \brief The ModbusTcpScanner class
///
class ModbusTcpScanner : public ModbusScanner
{
    Q_OBJECT
public:
    explicit ModbusTcpScanner(const ScanParams& params, QObject *parent = nullptr);

    void startScan() override;
    void stopScan() override;

signals:
    void scanNext(QPrivateSignal);

private slots:
    void on_scanNext(QPrivateSignal);

private:
    void processSocket(QTcpSocket* sck, const ConnectionDetails& cd);
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(QModbusTcpClient* client, int deviceId);

private:
    const ScanParams _params;
    int _processedSocketCount;
    QStack<ConnectionDetails> _connParams;
};

#endif // MODBUSTCPSCANNER_H
