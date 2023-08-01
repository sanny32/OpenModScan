#ifndef MODBUSRTUSCANNER_H
#define MODBUSRTUSCANNER_H

#include "modbusscanner.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QModbusRtuSerialMaster>
    typedef QModbusRtuSerialMaster QModbusRtuSerialClient;
#else
#include <QModbusRtuSerialClient>
#endif

class ModbusRtuScanner : public ModbusScanner
{
    Q_OBJECT
public:
    explicit ModbusRtuScanner(const ScanParams& params, QObject* parent = nullptr);

    void startScan() override;
    void stopScan() override;

private slots:
    void on_errorOccurred(QModbusDevice::Error error);
    void on_stateChanged(QModbusDevice::State state);

private:
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(int deviceId);

private:
    QModbusRtuSerialClient* _modbusClient;

private:
    const ScanParams _params;
    QList<ConnectionDetails>::ConstIterator _iterator;
};

#endif // MODBUSRTUSCANNER_H
