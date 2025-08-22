#ifndef SERIALPORTUTILS_H
#define SERIALPORTUTILS_H

#include <QDir>
#include <QSerialPortInfo>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")
#endif

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
        WCHAR deviceID[MAX_PATH];
        if (SetupDiGetDeviceInstanceIdW(hDevInfo, &devInfoData, deviceID, MAX_PATH, NULL))
        {
            QString deviceIdStr = QString::fromWCharArray(deviceID);
            if (deviceIdStr.contains(portName, Qt::CaseInsensitive))
            {
                WCHAR friendlyName[256];
                DWORD size = sizeof(friendlyName);
                if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)friendlyName, size, &size))
                {
                    QString friendlyNameStr = QString::fromWCharArray(friendlyName);
                    if (friendlyNameStr.contains("USB", Qt::CaseInsensitive) ||
                        friendlyNameStr.contains("Virtual", Qt::CaseInsensitive) ||
                        friendlyNameStr.contains("COMposite", Qt::CaseInsensitive))
                    {
                        isVirtual = true;
                        break;
                    }
                }

                // check through hardware ID
                WCHAR hardwareID[1024];
                if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareID, sizeof(hardwareID), &size))
                {
                    QString hardwareIdStr = QString::fromWCharArray(hardwareID);
                    if (hardwareIdStr.contains("USB", Qt::CaseInsensitive) ||
                        hardwareIdStr.contains("VID_", Qt::CaseInsensitive) ||
                        hardwareIdStr.contains("PID_", Qt::CaseInsensitive))
                    {
                        isVirtual = true;
                        break;
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
