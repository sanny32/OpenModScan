# OpenModScan

[![GitHub all releases](https://img.shields.io/github/downloads/sanny32/OpenModScan/total?logo=github&cacheSeconds=3600)](https://github.com/sanny32/OpenModScan/releases)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/sanny32/OpenModScan?logo=github&cacheSeconds=3600)](https://github.com/sanny32/OpenModScan/releases/latest)
[![License](https://img.shields.io/github/license/sanny32/OpenModScan)](LICENSE.md)

OpenModScan is a free and open-source Modbus Master (Client) utility supporting both Modbus-TCP and Modbus-RTU protocols.

<img width="1292" height="759" alt="изображение" src="https://github.com/user-attachments/assets/9454af59-2c08-4a62-9706-0fb1e668e67b" />

<img width="1292" height="759" alt="изображение" src="https://github.com/user-attachments/assets/bc0cdbd3-0a67-4477-b09f-f75ced9dfa12" />


## Features

The following Modbus functions are available:

Discrete Coils/Flags

    0x01 - Read Coils
    0x02 - Read Discrete Inputs
    0x05 - Write Single Coil
    0x0F - Write Multiple Coils

Registers

    0x03 - Read Holding Registers
    0x04 - Read Input Registers
    0x06 - Write Single Register
    0x10 - Write Multiple Registers
    0x16 - Mask Write Register

## Modbus Logging

<img width="1292" height="759" alt="изображение" src="https://github.com/user-attachments/assets/23226306-85d1-464d-ad74-4c007665c7ef" />

    
## Extended Featues
- Modbus Address Scan
  
  ![image](https://github.com/user-attachments/assets/798ed74a-c5fc-413f-a173-e223c9c7d3f3)

- Modbus Scanner (support protocols: Modbus RTU, Modbus TCP and Modbus RTU over TCP)
  
  <img width="803" height="719" alt="image" src="https://github.com/user-attachments/assets/77f14c57-2a13-42d4-a8a2-e718521a40c2" />

- Modbus Message Parser

  <img width="674" height="463" alt="image" src="https://github.com/user-attachments/assets/9d7a53ef-dda2-4d7f-bfb0-c3337e2ca40b" />

- Modbus User Message

  <img width="529" height="722" alt="image" src="https://github.com/user-attachments/assets/1aa3aadb-1462-4989-afa9-0ad131d5faa7" />


# Building
  Building is available via cmake (with installed Qt version 5.15 and above) or Qt Creator. Supports both OS Microsoft Windows and Linux.

## Microsoft Windows Building

The minimum supported version of Microsoft Windows for building OpenModScan from sources is Windows 10.

1. Install latest [git](https://git-scm.com/downloads/win) version
2. Run Windows PowerShell terminal as an Administrator
3. Clone OpenModScan sources from github repository
```powershell
git clone https://github.com/sanny32/OpenModScan.git
```
4. Go to OpenModScan folder
```powershell
cd OpenModScan
```
5. Allow script execution in PowerShell terminal
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```
6. Run the build script
```powershell
.\build.ps1
```
If you need to specify Qt framework major version (5 or 6), you can do it in the parameters
  - `.\build.ps1 -qt5` or  `.\build.ps1 -qt6`

The build script supports building the application only for 64-bit architecture.

## Linux Building
1. Install [git](https://git-scm.com/downloads/linux) for your Linux distribution
2. Run the following commands from the console:
```bash
git clone https://github.com/sanny32/OpenModScan.git
```
```bash
cd OpenModScan
```
```bash
./build.sh
```

If you need to specify Qt framework major version (5 or 6), you can do it in the parameters
  - `./build.sh -qt5` or  `./build.sh -qt6`

# About supported operating systems

The following minimum operating system versions are supported for OpenModScan:

- <img src="https://upload.wikimedia.org/wikipedia/ru/archive/3/33/20200302132354%21Windows_7_logo.svg" width="16" height="16"> **Microsoft Windows 7**
- <img src="https://upload.wikimedia.org/wikipedia/commons/4/4a/Debian-OpenLogo.svg" width="16" height="16"> **Debian Linux 11**
- <img src="https://upload.wikimedia.org/wikipedia/commons/9/9e/UbuntuCoF.svg" width="16" height="16"> **Ubuntu Linux 22.04**
- <img width="16" height="16" src="https://github.com/user-attachments/assets/bf804432-d951-47e0-8f75-52835a2c723f" /> **Mint Linux 22**
- <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/14/Zorin_Logomark.svg/120px-Zorin_Logomark.svg.png" width="16" height="16"> **Zorin OS 18**
- <img src="https://upload.wikimedia.org/wikipedia/commons/4/41/Fedora_icon_%282021%29.svg" width="16" height="16"> **Fedora Linux 41**
- <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/77/Rocky_Linux_logo.svg/960px-Rocky_Linux_logo.svg.png" width="16" height="16"> **Rocky Linux 9.7**
- <img src="https://upload.wikimedia.org/wikipedia/commons/d/d0/OpenSUSE_Logo.svg" width="16" height="16"> **OpenSuse Linux 15.6**
- <img src="https://www.altlinux.org/Images.www.altlinux.org/c/c5/Logo_alt_company.png" width="16" height="16"> **Alt Linux 11**
- <img width="16" height="16" src="https://github.com/user-attachments/assets/7d52237f-a732-4973-b292-c5834fdc809c" /> **Astra Linux 1.7**
- <img width="16" height="16" src="https://github.com/user-attachments/assets/e8a6e06e-b43f-42b4-8d8d-fea4aa858cb0" /> **RedOS 8**

# Install from [binary distributions](https://github.com/sanny32/OpenModScan/releases/latest)

Below are the methods for installing the OpenModScan for different OS

## <img src="https://upload.wikimedia.org/wikipedia/commons/8/87/Windows_logo_-_2021.svg" width="16" height="16"> Microsoft Windows
Run the installer:

- For 32-bit Windows: `qt5-omodscan_1.13.1_x86.exe`
- For 64-bit Windows: `qt5-omodscan_1.13.1_x64.exe` or `qt6-omodscan_1.13.1_x64.exe`


## <img src="https://upload.wikimedia.org/wikipedia/commons/4/4a/Debian-OpenLogo.svg" width="20" height="20"> Debian / <img src="https://upload.wikimedia.org/wikipedia/commons/9/9e/UbuntuCoF.svg" width="20" height="20"> Ubuntu / <img width="20" height="20" src="https://github.com/user-attachments/assets/bf804432-d951-47e0-8f75-52835a2c723f" /> Mint / <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/14/Zorin_Logomark.svg/120px-Zorin_Logomark.svg.png" width="20" height="20"> Zorin / <img width="22" height="22" src="https://github.com/user-attachments/assets/7d52237f-a732-4973-b292-c5834fdc809c" /> Astra Linux
### Install
Install the DEB package from the command line:
```bash
sudo apt install ./qt6-omodscan_1.13.1-1_amd64.deb
```
or if you want to use Qt5 libraries:
```bash
sudo apt install ./qt5-omodscan_1.13.1-1_amd64.deb
```

### Remove
To remove the DEB package run:
```bash
sudo apt remove qt6-omodscan
```
or for Qt5 package:
```bash
sudo apt remove qt5-omodscan
```

## <img src="https://upload.wikimedia.org/wikipedia/commons/d/d8/Red_Hat_logo.svg" width="24" height="24"> RedHat / <img src="https://upload.wikimedia.org/wikipedia/commons/4/41/Fedora_icon_%282021%29.svg" width="20" height="20"> Fedora / <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/77/Rocky_Linux_logo.svg/960px-Rocky_Linux_logo.svg.png" width="20" height="20"> Rocky / <img width="20" height="20" src="https://github.com/user-attachments/assets/e8a6e06e-b43f-42b4-8d8d-fea4aa858cb0" /> RedOS Linux
### Install
Install the RPM package from the command line:
```bash
sudo dnf install ./qt6-omodscan_1.13.1-1.x86_64.rpm
```

### Remove
To remove the RPM package run:
```bash
sudo dnf remove qt6-omodscan
```

## <img src="https://www.altlinux.org/Images.www.altlinux.org/c/c5/Logo_alt_company.png" width="24" height="24"> Alt Linux
### Install
Install the RPM package from the command line as root user:
```bash
apt-get install ./qt6-omodscan_1.13.1-1.x86_64.rpm
```

### Remove
To remove the RPM package run as root user:
```bash
apt-get remove qt6-omodscan
```

## <img width="20" height="20" src="https://github.com/user-attachments/assets/04967844-610a-493e-ac9b-b21aa6b88e65" /> SUSE / <img src="https://upload.wikimedia.org/wikipedia/commons/d/d0/OpenSUSE_Logo.svg" width="24" height="24" /> OpenSUSE Linux
### Install
Import qt6-omodscan.rpm.pubkey to rpm repository:
```bash
sudo rpm --import qt6-omodscan.rpm.pubkey
```
Install the RPM package using Zypper:
```bash
sudo zypper install ./qt6-omodscan_1.13.1-1.x86_64.rpm
```

### Remove
To remove the RPM package run:
```bash
sudo zypper remove qt6-omodscan
```

## <img width="18" height="20" src="https://github.com/user-attachments/assets/c5cede56-a6be-421a-8611-e99f7c9a3b1e" /> Flatpak
### Install
Install flatpak package run the followning commands:
```bash
flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
```
```bash
flatpak install --user io.github.sanny32.omodscan.flatpak
```

If you want to use a serial port connection, you must add the user to the `dialout` group
```bash
sudo usermod -a -G dialout $USER
```
and then log in to the user again or reboot the computer.

### Remove
To remove the flatpak package run:
```bash
flatpak uninstall --user io.github.sanny32.omodscan
```
# Code Signing Sponsor
Windows binaries for this project are signed thanks to **SignPath Foundation**.
*   🔐 **Free code signing service:** [SignPath.io](https://signpath.io)
*   📜 **Certificate provider:** [SignPath Foundation](https://signpath.org)
  
# MIT License
Copyright 2023-2026 Alexandr Ananev [mail@ananev.org]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


