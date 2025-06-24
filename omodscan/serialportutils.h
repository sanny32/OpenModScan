#ifndef SERIALPORTUTILS_H
#define SERIALPORTUTILS_H

#include <QDir>
#include <QSerialPortInfo>
#include <QRegularExpression>

///
/// \brief isVirtualPort
/// \param portName
/// \return
///
inline bool isVirtualPort(const QString& portName)
{
#ifdef Q_OS_LINUX
    QFileInfo info("/sys/class/tty/" + portName);
    QString sysPath = info.symLinkTarget(); // /sys/devices/...

    if (sysPath.isEmpty())
        return false;

    // Проверяем, содержит ли путь "serial8250"
    return sysPath.contains("platform/serial8250");
#else
    Q_UNUSED(portName);
    return false;
#endif
}


///
/// \brief getAvailableSerialPorts
/// \param excludeVirtuals
/// \return
///
inline QStringList getAvailableSerialPorts(bool excludeVirtuals = false)
{
    QStringList ports;
    for(auto&& port: QSerialPortInfo::availablePorts()) {
        if(excludeVirtuals && isVirtualPort(port.portName())) {
            continue;
        }

        ports << port.portName();
    }

    static QRegularExpression re( "[^\\d]");
    std::sort(ports.begin(), ports.end(), [](QString p1, QString p2)
              {
                  return p1.remove(re).toInt() < p2.remove(re).toInt();
              });

    return ports;
}

#endif // SERIALPORTUTILS_H
