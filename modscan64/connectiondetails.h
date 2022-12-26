#ifndef CONNECTIONDETAILS_H
#define CONNECTIONDETAILS_H

#include <QHostAddress>
#include <QSerialPort>
#include "enums.h"

struct ConnectionDetails
{
    ConnectionType Type;
    ushort ServicePort;
    QHostAddress IPAddress;
    QSerialPort SerialPort;
};

#endif // CONNECTIONDETAILS_H
