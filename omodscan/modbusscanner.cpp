#include "modbusscanner.h"

///
/// \brief ModbusScanner::ModbusScanner
/// \param parent
///
ModbusScanner::ModbusScanner(QObject *parent)
    : QObject{parent}
{
    connect(&_timer, &QTimer::timeout, this, &ModbusScanner::on_timeout);
}

///
/// \brief ModbusScanner::startScan
///
void ModbusScanner::startScan()
{
    _scanTime = 0;
    _inProgress = true;
    _timer.start(1000);
}

///
/// \brief ModbusScanner::stopScan
///
void ModbusScanner::stopScan()
{
    _inProgress = false;
    _timer.stop();

    emit finished();
}

///
/// \brief ModbusScanner::inProgress
/// \return
///
bool ModbusScanner::inProgress() const
{
    return _inProgress;
}

///
/// \brief ModbusScanner::modbusRequest
/// \return
///
QModbusRequest ModbusScanner::modbusRequest() const
{
    return QModbusRequest(QModbusPdu::ReportServerId);
}

///
/// \brief ModbusScanner::on_timeout
///
void ModbusScanner::on_timeout()
{
    emit timeout(++_scanTime);
}
