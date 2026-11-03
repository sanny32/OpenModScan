#include "modbusreply.h"

///
/// \brief ModbusReply::ModbusReply
/// \param type
/// \param serverAddress
/// \param parent
///
ModbusReply::ModbusReply(ReplyType type, int serverAddress, QObject *parent)
    : QObject{parent}
    ,_type(type)
    ,_serverAddress(serverAddress)
{
}

///
/// \brief ModbusReply::isFinished
/// \return
///
bool ModbusReply::isFinished() const
{
    return _finished;
}

///
/// \brief ModbusReply::setFinished
/// \param isFinished
///
void ModbusReply::setFinished(bool isFinished)
{
    _finished = isFinished;
    if (isFinished)
        emit finished();
}

///
/// \brief ModbusReply::result
/// \return
///
QModbusDataUnit ModbusReply::result() const
{
    if (type() != ModbusReply::Broadcast)
        return _unit;
    return QModbusDataUnit();
}

///
/// \brief ModbusReply::setResult
/// \param unit
///
void ModbusReply::setResult(const QModbusDataUnit &unit)
{
    _unit = unit;
}

///
/// \brief ModbusReply::serverAddress
/// \return
///
int ModbusReply::serverAddress() const
{
    return _serverAddress;
}

///
/// \brief ModbusReply::error
/// \return
///
ModbusDevice::Error ModbusReply::error() const
{
    return _error;
}

///
/// \brief ModbusReply::setError
/// \param error
/// \param errorText
///
void ModbusReply::setError(ModbusDevice::Error error, const QString &errorText)
{
    _error = error;
    _errorText = errorText;
    emit errorOccurred(error);
    setFinished(true);
}

///
/// \brief ModbusReply::errorString
/// \return
///
QString ModbusReply::errorString() const
{
    return _errorText;
}

///
/// \brief ModbusReply::requestGroupId
/// \return
///
int ModbusReply::requestGroupId() const
{
    return _requestGroupId;
}

///
/// \brief ModbusReply::setRequestGroupId
/// \param value
///
void ModbusReply::setRequestGroupId(int value)
{
    _requestGroupId = value;
}

///
/// \brief ModbusReply::transactionId
/// \return
///
int ModbusReply::transactionId() const
{
    return _transactionId;
}

///
/// \brief ModbusReply::setTransactionId
/// \param value
///
void ModbusReply::setTransactionId(int value)
{
    _transactionId = value;
}

///
/// \brief ModbusReply::type
/// \return
///
ModbusReply::ReplyType ModbusReply::type() const
{
    return _type;
}

///
/// \brief ModbusReply::rawResult
/// \return
///
QModbusResponse ModbusReply::rawResult() const
{
    return _response;
}

///
/// \brief ModbusReply::setRawResult
/// \param response
///
void ModbusReply::setRawResult(const QModbusResponse &response)
{
    _response = response;
}

///
/// \brief ModbusReply::intermediateErrors
/// \return
///
QList<ModbusDevice::IntermediateError> ModbusReply::intermediateErrors() const
{
    return _intermediateErrors;
}

///
/// \brief ModbusReply::addIntermediateError
/// \param error
///
void ModbusReply::addIntermediateError(ModbusDevice::IntermediateError error)
{
    _intermediateErrors.append(error);
    emit intermediateErrorOccurred(error);
}
