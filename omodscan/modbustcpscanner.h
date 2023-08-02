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

private:
    void connectDevice(const ConnectionDetails& params);
    void sendRequest(QModbusTcpClient* client, int deviceId);

private:
    const ScanParams _params;
};

#endif // MODBUSTCPSCANNER_H
