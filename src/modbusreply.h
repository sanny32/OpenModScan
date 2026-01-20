#ifndef MODBUSREPLY_H
#define MODBUSREPLY_H

#include <QModbusDataUnit>
#include <QModbusResponse>
#include "modbusdevice.h"

///
/// \brief The ModbusReply class
///
class ModbusReply : public QObject
{
    Q_OBJECT
public:

    enum ReplyType {
        Raw,
        Common,
        Broadcast
    };
    Q_ENUM(ReplyType)

    ModbusReply(ReplyType type, int serverAddress, QObject *parent = nullptr);

    ReplyType type() const;
    int serverAddress() const;

    bool isFinished() const;

    QModbusDataUnit result() const;
    QModbusResponse rawResult() const;

    QString errorString() const;
    ModbusDevice::Error error() const;

    int requestGroupId() const;
    void setRequestGroupId(int value);

    int transactionId() const;
    void setTransactionId(int value);

    void setResult(const QModbusDataUnit &unit);
    void setRawResult(const QModbusResponse &unit);

    void setFinished(bool isFinished);
    void setError(ModbusDevice::Error error, const QString &errorText);

    QList<ModbusDevice::IntermediateError> intermediateErrors() const;
    void addIntermediateError(ModbusDevice::IntermediateError error);

signals:
    void finished();
    void errorOccurred(ModbusDevice::Error error);
    void intermediateErrorOccurred(ModbusDevice::IntermediateError error);

private:
    QModbusDataUnit _unit;
    int _serverAddress = 1;
    int _requestGroupId = 0;
    int _transactionId = 0;
    bool _finished = false;
    ModbusDevice::Error _error = ModbusDevice::NoError;
    QString _errorText;
    QModbusResponse _response;
    ModbusReply::ReplyType _type;
    QList<ModbusDevice::IntermediateError> _intermediateErrors;
};
Q_DECLARE_TYPEINFO(ModbusReply::ReplyType, Q_PRIMITIVE_TYPE);

#endif // MODBUSREPLY_H
