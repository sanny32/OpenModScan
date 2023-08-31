#ifndef MODBUSTCPSCANNER_H
#define MODBUSTCPSCANNER_H

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
    void scanDeviceFinished(QPrivateSignal);

private slots:
    void on_scanDeviceFinished(QPrivateSignal);

private:
    void internalScan();
    void processSocket(QTcpSocket* sck, const ConnectionDetails& cd);
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(QModbusTcpClient* client, int deviceId);

private:
    int _processedSocketCount;
    QVector<ConnectionDetails> _itemsToScan;
    QVector<ConnectionDetails>::ConstIterator _iterator;
    const ScanParams _params;
};

#endif // MODBUSTCPSCANNER_H
