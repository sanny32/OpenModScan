#ifndef CONNECTIONDETAILS_H
#define CONNECTIONDETAILS_H

#include <QHostAddress>
#include <QSerialPort>
#include <QModbusDevice>
#include "enums.h"

///
/// \brief The TcpConnectionParams class
///
struct TcpConnectionParams
{
    quint16 ServicePort = 502;
    QString IPAddress = "127.0.0.1";
};

///
/// \brief The SerialConnectionParams class
///
struct SerialConnectionParams
{
    QString PortName;
    QSerialPort::BaudRate BaudRate = QSerialPort::Baud9600;
    QSerialPort::DataBits WordLength = QSerialPort::Data8;
    QSerialPort::Parity Parity = QSerialPort::NoParity;
    QSerialPort::StopBits StopBits = QSerialPort::OneStop;
    QSerialPort::FlowControl FlowControl = QSerialPort::NoFlowControl;
    bool SetDTR = true;
    bool SetRTS = true;
};

///
/// \brief The ModbusProtocolSelections class
///
struct ModbusProtocolSelections
{
    TransmissionMode Mode = TransmissionMode::RTU;
    quint32 SlaveResponseTimeOut = 250;
    quint32 DelayBetweenPolls = 0;
    bool ForceModbus15And16Func = false;
};

///
/// \brief The ConnectionDetails class
///
struct ConnectionDetails
{
    ConnectionType Type = ConnectionType::Tcp;
    TcpConnectionParams TcpParams;
    SerialConnectionParams SerialParams;
    ModbusProtocolSelections ModbusParams;
};

#endif // CONNECTIONDETAILS_H
