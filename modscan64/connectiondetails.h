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
    ushort ServicePort = 502;
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
    bool WaitDSR = false;
    bool WaitCTS = false;
    uint DelayDSR = 0;
    uint DelayCTS = 0;
};

///
/// \brief The ModbusProtocolSelections class
///
struct ModbusProtocolSelections
{
    TransmissionMode Mode = TransmissionMode::RTU;
    uint SlaveResponseTimeOut = 250;
    uint DelayBetweenPolls = 0;
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
