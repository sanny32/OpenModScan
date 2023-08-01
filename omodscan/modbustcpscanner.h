#ifndef MODBUSTCPSCANNER_H
#define MODBUSTCPSCANNER_H

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

private slots:
    void on_errorOccurred(QModbusDevice::Error error);
    void on_stateChanged(QModbusDevice::State state);

private:
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(int deviceId);

private:
    QModbusTcpClient* _modbusClient;

private:
    const ScanParams _params;
    QList<ConnectionDetails>::ConstIterator _iterator;
};

#endif // MODBUSTCPSCANNER_H
