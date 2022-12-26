#ifndef CONNECTIONDETAILS_H
#define CONNECTIONDETAILS_H

#include <QHostAddress>
#include <QSerialPort>
#include <QModbusDevice>
#include "enums.h"

///
/// \brief The TcpConnectionDetails class
///
struct TcpConnectionDetails
{
    ushort ServicePort = 502;
    QHostAddress IPAddress = QHostAddress("127.0.0.1");
};

///
/// \brief The SerialConnectionDetails class
///
struct SerialConnectionDetails
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
    TcpConnectionDetails TcpDetails;
    SerialConnectionDetails SerialDetails;
    ModbusProtocolSelections ProtocolSelections;
};

#endif // CONNECTIONDETAILS_H
