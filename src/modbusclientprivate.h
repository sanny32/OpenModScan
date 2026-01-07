#ifndef MODBUSCLIENTPRIVATE_H
#define MODBUSCLIENTPRIVATE_H

#include <QTimer>
#include <QPointer>
#include <QSharedPointer>
#include <QLoggingCategory>
#include <QModbusPdu>
#include "modbusreply.h"
#include "modbusmessage.h"

enum Coil {
    On = 0xff00,
    Off = 0x0000
};

namespace Diagnostics {
enum SubFunctionCode {
    ReturnQueryData = 0x0000,
    RestartCommunicationsOption = 0x0001,
    ReturnDiagnosticRegister = 0x0002,
    ChangeAsciiInputDelimiter = 0x0003,
    ForceListenOnlyMode = 0x0004,
    ClearCountersAndDiagnosticRegister = 0x000a,
    ReturnBusMessageCount = 0x000b,
    ReturnBusCommunicationErrorCount = 0x000c,  // CRC error counter
    ReturnBusExceptionErrorCount = 0x000d,
    ReturnServerMessageCount = 0x000e,
    ReturnServerNoResponseCount = 0x000f,
    ReturnServerNAKCount = 0x0010,
    ReturnServerBusyCount = 0x0011,
    ReturnBusCharacterOverrunCount = 0x0012,
    ClearOverrunCounterAndFlag = 0x0014
};
}

///
/// \brief The ModbusClientPrivate class
///
class ModbusClientPrivate : public QObject
{
    Q_OBJECT

public:
    virtual QVariant connectionParameter(ModbusDevice::ConnectionParameter parameter) const = 0;
    virtual void setConnectionParameter(ModbusDevice::ConnectionParameter parameter, const QVariant& value) = 0;

    bool connectDevice();
    void disconnectDevice();

    virtual bool isOpen() const { return false; }

    ModbusDevice::State state() const;

    ModbusDevice::Error error() const;
    QString errorString() const;

    int timeout() const;
    void setTimeout(int newTimeout);

    int numberOfRetries() const;
    void setNumberOfRetries(int number);

    ModbusReply* sendReadRequest(const QModbusDataUnit& read, int serverAddress, int requestGroupId = 0);
    ModbusReply* sendWriteRequest(const QModbusDataUnit& write, int serverAddress, int requestGroupId = 0);
    ModbusReply* sendReadWriteRequest(const QModbusDataUnit& read, const QModbusDataUnit& write, int serverAddress, int requestGroupId = 0);
    ModbusReply* sendRawRequest(const QModbusRequest& request, int serverAddress, int requestGroupId = 0);

    virtual QIODevice *device() const = 0;

signals:
    void timeoutChanged(int newTimeout);
    void errorOccurred(ModbusDevice::Error error);
    void stateChanged(ModbusDevice::State state);

    void modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg);

protected:
    explicit ModbusClientPrivate(QObject *parent = nullptr);

    virtual bool open() = 0;
    virtual void close() = 0;

    void setState(ModbusDevice::State newState);
    void setError(const QString& errorText, ModbusDevice::Error error);

    ModbusReply* sendRequest(int requestGroupId, const QModbusRequest& request, int serverAddress, const QModbusDataUnit* const unit);
    QModbusRequest createReadRequest(const QModbusDataUnit& data) const;
    QModbusRequest createWriteRequest(const QModbusDataUnit& data) const;
    QModbusRequest createRWRequest(const QModbusDataUnit& read, const QModbusDataUnit& write) const;

    virtual bool processResponse(const QModbusResponse& response, QModbusDataUnit* data);
    virtual bool processPrivateResponse(const QModbusResponse& response, QModbusDataUnit* data);

    bool processReadCoilsResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool processReadDiscreteInputsResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool collateBits(const QModbusPdu& pdu, QModbusDataUnit::RegisterType type, QModbusDataUnit* data);

    bool processReadHoldingRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool processReadInputRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool collateBytes(const QModbusPdu& pdu, QModbusDataUnit::RegisterType type, QModbusDataUnit* data);

    bool processWriteSingleCoilResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool processWriteSingleRegisterResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool collateSingleValue(const QModbusPdu& pdu, QModbusDataUnit::RegisterType type, QModbusDataUnit* data);

    bool processWriteMultipleCoilsResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool processWriteMultipleRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data);
    bool collateMultipleValues(const QModbusPdu &pdu, QModbusDataUnit::RegisterType type, QModbusDataUnit* data);
    bool processReadWriteMultipleRegistersResponse(const QModbusResponse& response, QModbusDataUnit* data);

    virtual ModbusReply* enqueueRequest(int requestGroupId, const QModbusRequest&, int, const QModbusDataUnit&, ModbusReply::ReplyType) {
        return nullptr;
    }

    struct QueueElement {
        QueueElement() = default;
        QueueElement(ModbusReply* r, const QModbusRequest& req, const QModbusDataUnit& u, int num, int timeout = -1)
            : reply(r), requestPdu(req), unit(u), numberOfRetries(num)
        {
            if (timeout >= 0) {
                // always the case for TCP
                timer = QSharedPointer<QTimer>::create();
                timer->setSingleShot(true);
                timer->setInterval(timeout);
            }
        }
        bool operator==(const QueueElement &other) const {
            return reply == other.reply;
        }

        QPointer<ModbusReply> reply;
        QModbusRequest requestPdu;
        QModbusDataUnit unit;
        int numberOfRetries;
        QSharedPointer<QTimer> timer;
        QByteArray adu;
        qint64 bytesWritten = 0;
        qint32 m_timerId = INT_MIN;
    };
    void processQueueElement(const QModbusResponse& pdu, ModbusMessage::ProtocolType protocolType, int serverAddress, const QueueElement& element);

private:
    int _numberOfRetries = 3;
    int _responseTimeoutDuration = 1000;

    ModbusDevice::State _state = ModbusDevice::UnconnectedState;
    ModbusDevice::Error _error = ModbusDevice::NoError;
    QString _errorString;
};

Q_DECLARE_LOGGING_CATEGORY(QT_MODBUS)
Q_DECLARE_LOGGING_CATEGORY(QT_MODBUS_LOW)


#endif // MODBUSCLIENTPRIVATE_H
