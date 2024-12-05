#ifndef SERIALPORTUTILS_H
#define SERIALPORTUTILS_H

#include <QSerialPortInfo>
#include <QRegularExpression>

///
/// \brief getAvailableSerialPorts
/// \return
///
inline QStringList getAvailableSerialPorts()
{
    QStringList ports;
    for(auto&& port: QSerialPortInfo::availablePorts())
        ports << port.portName();

    static QRegularExpression re( "[^\\d]");
    std::sort(ports.begin(), ports.end(), [](QString p1, QString p2)
              {
                  return p1.remove(re).toInt() < p2.remove(re).toInt();
              });

    return ports;
}

#endif // SERIALPORTUTILS_H
