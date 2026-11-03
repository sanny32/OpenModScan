#ifndef MODBUSRTUSCANNER_H
#define MODBUSRTUSCANNER_H

#include "modbusscanner.h"
#include "modbusrtuclient.h"

class ModbusRtuScanner : public ModbusScanner
{
    Q_OBJECT
public:
    explicit ModbusRtuScanner(const ScanParams& params, QObject* parent = nullptr);

    void startScan() override;
    void stopScan() override;

private slots:
    void on_errorOccurred(ModbusDevice::Error error);
    void on_stateChanged(ModbusDevice::State state);

private:
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(int deviceId);

private:
    ModbusRtuClient* _modbusClient;

private:
    const ScanParams _params;
    QList<ConnectionDetails>::ConstIterator _iterator;
};

#endif // MODBUSRTUSCANNER_H
