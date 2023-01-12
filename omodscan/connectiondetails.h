#ifndef CONNECTIONDETAILS_H
#define CONNECTIONDETAILS_H

#include <QHostAddress>
#include <QSerialPort>
#include <QModbusDevice>
#include <QDataStream>
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
    quint32 NumberOfRetries = 3;
    quint32 InterFrameDelay = 0;
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

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const TcpConnectionParams& params)
{
    out << params.IPAddress;
    out << params.ServicePort;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, TcpConnectionParams& params)
{
    in >> params.IPAddress;
    in >> params.ServicePort;

    return in;
}

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const SerialConnectionParams& params)
{
    out << params.PortName;
    out << params.BaudRate;
    out << params.WordLength;
    out << params.Parity;
    out << params.StopBits;
    out << params.FlowControl;
    out << params.SetDTR;
    out << params.SetRTS;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, SerialConnectionParams& params)
{
    in >> params.PortName;
    in >> params.BaudRate;
    in >> params.WordLength;
    in >> params.Parity;
    in >> params.StopBits;
    in >> params.FlowControl;
    in >> params.SetDTR;
    in >> params.SetRTS;

    return in;
}

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const ModbusProtocolSelections& params)
{
    out << params.Mode;
    out << params.SlaveResponseTimeOut;
    out << params.NumberOfRetries;
    out << params.InterFrameDelay;
    out << params.ForceModbus15And16Func;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, ModbusProtocolSelections& params)
{
    in >> params.Mode;
    in >> params.SlaveResponseTimeOut;
    in >> params.NumberOfRetries;
    in >> params.InterFrameDelay;
    in >> params.ForceModbus15And16Func;

    return in;
}

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const ConnectionDetails& params)
{
    out << params.Type;
    out << params.TcpParams;
    out << params.SerialParams;
    out << params.ModbusParams;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, ConnectionDetails& params)
{
    in >> params.Type;
    in >> params.TcpParams;
    in >> params.SerialParams;
    in >> params.ModbusParams;

    return in;
}


#endif // CONNECTIONDETAILS_H
