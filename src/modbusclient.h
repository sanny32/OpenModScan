#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QModbusClient>
#include "modbusmessage.h"
#include "connectiondetails.h"
#include "modbuswriteparams.h"
#include "modbusclientprivate.h"

///
/// \brief The ModbusClient class
///
class ModbusClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusClient(QObject *parent = nullptr);
    ~ModbusClient() override;

    void connectDevice(const ConnectionDetails& cd);
    void disconnectDevice();

    bool isValid() const;
    ModbusDevice::State state() const;

    ConnectionType connectionType() const {
        return _connectionType;
    }

    bool isForcedModbus15And16Func() const;

    int timeout() const;
    void setTimeout(int newTimeout);

    uint numberOfRetries() const;
    void setNumberOfRetries(uint number);

    void sendRawRequest(const QModbusRequest& request, int server, int requestGroupId);
    void sendReadRequest(QModbusDataUnit::RegisterType pointType, int startAddress, quint16 valueCount, int server, int requestGroupId);
    void writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params, int requestGroupId);
    void maskWriteRegister(const ModbusMaskWriteParams& params, int requestGroupId);

    quint16 syncReadRegister(QModbusDataUnit::RegisterType pointType, int address, int server);

signals:
    void modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void modbusReply(const ModbusReply* const reply);
    void modbusError(const QString& error, int requestGroupId);
    void modbusConnectionError(const QString& error);
    void modbusConnecting(const ConnectionDetails& cd);
    void modbusConnected(const ConnectionDetails& cd);
    void modbusDisconnected(const ConnectionDetails& cd);

private slots:
    void on_readReply();
    void on_writeReply();
    void on_errorOccurred(ModbusDevice::Error error);
    void on_stateChanged(ModbusDevice::State state);

private:
    ModbusClientPrivate* _modbusClient;
    ConnectionType _connectionType;
};

Q_DECLARE_METATYPE(QModbusDataUnit)
DECLARE_ENUM_STRINGS(QModbusDataUnit::RegisterType,
                     {   QModbusDataUnit::Invalid,          "Invalid"           },
                     {   QModbusDataUnit::DiscreteInputs,   "DiscreteInputs"    },
                     {   QModbusDataUnit::Coils,            "Coils"             },
                     {   QModbusDataUnit::InputRegisters,   "InputRegisters"    },
                     {   QModbusDataUnit::HoldingRegisters, "HoldingRegisters"  }
)

#endif // MODBUSCLIENT_H
