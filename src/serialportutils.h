#ifndef SERIALPORTUTILS_H
#define SERIALPORTUTILS_H

#include <QDir>
#include <QDebug>
#include <QSerialPortInfo>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")
#endif

///
/// \brief isVirt
/// \param n
/// \return
///
inline bool isVirt(const QString& n)
{
    return n.contains("USB", Qt::CaseInsensitive)  ||
           n.contains("VIRT", Qt::CaseInsensitive) ||
           n.contains("ROOT", Qt::CaseInsensitive);
}

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

    // Check device path "serial8250"
    return sysPath.contains("platform/serial8250");
#elif defined(Q_OS_WIN)
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, NULL, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return false;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    bool isVirtual = false;
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        wchar_t friendlyName[256];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME,
                                             nullptr, (PBYTE)friendlyName, sizeof(friendlyName), nullptr))
        {
            const QString fn = QString::fromWCharArray(friendlyName);
            if (fn.contains(portName, Qt::CaseInsensitive))
            {
                if (isVirt(fn))
                {
                    isVirtual = true;
                    break;
                }
                else
                {
                    wchar_t hwid[256];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_HARDWAREID,
                                                         nullptr, (PBYTE)hwid, sizeof(hwid), nullptr))
                    {
                        const QString id = QString::fromWCharArray(hwid);
                        if (isVirt(id))
                        {
                            isVirtual = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return isVirtual;
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
