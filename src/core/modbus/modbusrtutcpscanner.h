#ifndef MODBUSRTUTCPSCANNER_H
#define MODBUSRTUTCPSCANNER_H

#include "modbustcpscanner.h"
#include "modbusrtutcpclient.h"

///
/// \brief The ModbusRtuTcpScanner class
///
class ModbusRtuTcpScanner : public ModbusTcpScanner
{
    Q_OBJECT
public:
    explicit ModbusRtuTcpScanner(const ScanParams& params, QObject *parent = nullptr)
        : ModbusTcpScanner(params, parent)
    {
    }

protected:
    ModbusClientPrivate* createClient() override {
        return new ModbusRtuTcpClient(this);
    }
};

#endif // MODBUSRTUTCPSCANNER_H
