#ifndef SERIALPORTUTILS_H
#define SERIALPORTUTILS_H

#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSerialPortInfo>
#include <QRegularExpression>
#include <algorithm>

#ifdef Q_OS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#endif

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
#elif defined(Q_OS_MACOS)
    QString devicePath = portName.startsWith('/') ? portName : "/dev/" + portName;
    const QString canonicalPath = QFileInfo(devicePath).canonicalFilePath();
    if (!canonicalPath.isEmpty())
        devicePath = canonicalPath;

    CFMutableDictionaryRef matching = IOServiceMatching(kIOSerialBSDServiceValue);
    if (!matching)
        return false;

    const QByteArray path = devicePath.toUtf8();
    CFStringRef device = CFStringCreateWithCString(kCFAllocatorDefault, path.constData(),
                                                  kCFStringEncodingUTF8);
    if (!device) {
        CFRelease(matching);
        return false;
    }
    const CFStringRef key = devicePath.startsWith("/dev/tty.")
                               ? CFSTR(kIODialinDeviceKey) : CFSTR(kIOCalloutDeviceKey);
    CFDictionarySetValue(matching, key, device);
    CFRelease(device);

    // IOServiceGetMatchingService consumes the matching dictionary.
    io_service_t entry = IOServiceGetMatchingService(MACH_PORT_NULL, matching);
    if (!entry) {
        // POSIX pseudo terminals are not published in the I/O Registry.
        static const QRegularExpression ptyPath("^/dev/(ttys[0-9]+|[pt]ty[pqrs][0-9a-f])$");
        return !canonicalPath.isEmpty() && ptyPath.match(devicePath).hasMatch();
    }

    while (entry) {
        // Stop at the nearest transport, before reaching the host's own hardware.
        io_name_t className = {};
        IOObjectGetClass(entry, className);
        const bool isBluetooth = QByteArray(className).contains("Bluetooth");
        const bool isSoftware = IOObjectConformsTo(entry, "IOResources");
        // USB adapters remain hardware ports, including USB CDC virtual COM devices.
        const bool isHardware = IOObjectConformsTo(entry, "IOUSBHostDevice") ||
                                IOObjectConformsTo(entry, "IOUSBDevice") ||
                                IOObjectConformsTo(entry, "IOPCIDevice") ||
                                IOObjectConformsTo(entry, "IOACPIPlatformDevice") ||
                                IOObjectConformsTo(entry, "AppleARMIODevice");
        if (isBluetooth || isSoftware || isHardware) {
            IOObjectRelease(entry);
            return isBluetooth || isSoftware;
        }

        io_registry_entry_t parent = IO_OBJECT_NULL;
        const kern_return_t result = IORegistryEntryGetParentEntry(entry, kIOServicePlane, &parent);
        IOObjectRelease(entry);
        if (result != KERN_SUCCESS)
            return false;
        entry = parent;
    }
    return false;
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
