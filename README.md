# OpenModScan
OpenModScan is a free and open-source Modbus Master (Client) utility supporting both Modbus-TCP and Modbus-RTU protocols.

<img width="1292" height="759" alt="image" src="https://github.com/user-attachments/assets/fc839046-f9de-484a-ab8c-1b553635cba8" />


<img width="1292" height="760" alt="image" src="https://github.com/user-attachments/assets/f4e6599c-480d-4b26-b0c3-49e5dc8d1f17" />


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

<img width="1292" height="759" alt="image" src="https://github.com/user-attachments/assets/b12b019f-6df6-4c85-b7f3-85e347576c83" />


    
## Extended Featues
- Modbus Address Scan
  
  ![image](https://github.com/user-attachments/assets/798ed74a-c5fc-413f-a173-e223c9c7d3f3)

- Modbus Scanner (supports both Modbus RTU and Modbus TCP scanning)
  
  ![image](https://github.com/user-attachments/assets/17d5f43d-c341-455d-a9b8-67db50a35699)

- Modbus Message Parser

  <img width="674" height="463" alt="image" src="https://github.com/user-attachments/assets/9d7a53ef-dda2-4d7f-bfb0-c3337e2ca40b" />

- Modbus User Message

  <img width="529" height="722" alt="image" src="https://github.com/user-attachments/assets/1aa3aadb-1462-4989-afa9-0ad131d5faa7" />


# Building
  Building is available via cmake (with installed Qt version 5.15 and above) or Qt Creator. Supports both OS Microsoft Windows and Linux.

## Linux Building  
- `git clone https://github.com/sanny32/OpenModScan.git`
- `cd OpenModScan`
- `./build.sh`

If you need to specify Qt framework major version (5 or 6), you can do it in the parameters
  - `./build.sh -qt5` or  `./build.sh -qt6`

# About supported operating systems

The following operating systems are supported for OpenModScan.
- Microsoft Windows 7 x86 and amd64 or later
- Debian Linux 11 amd64 or later
- Ubuntu Linux 22.04 amd64 or later
- Mint Linux 22 amd64 or later
- Fedora Linux 41 amd64 or later
- OpenSuse Linux 15.6 amd64 or later
- Alt Linux 11 amd64 or later
- Astra Linux 1.7 amd64 or later
- RedOS 8 amd64 or later

# Install

Installing OpenModScan from binary distributions.

## Microsoft Windows
Run the installer:

- For 32-bit Windows: `qt5-omodscan_X.XX.X-Y_x86.exe`
- For 64-bit Windows: `qt5-omodscan_X.XX.X-Y_amd64.exe` or `qt6-omodscan_X.XX.X-Y_amd64.exe`

Replace `X.XX.X-Y` with the application version.

## Debian/Ubintu/Mint/Astra Linux
Install the DEB package from the command line:
```bash
sudo apt install -f ./qt5-omodscan_X.XX.X-Y_amd64.deb
```
or if you want to use Qt5 libraries:
```bash
sudo apt install -f ./qt6-omodscan_X.XX.X-Y_amd64.deb
```
Replace X.XX.X-Y with the application version.

## RedHat/Fedora/RedOS Linux
Install the RPM package from the command line:
```bash
sudo dnf install ./qt6-omodscan_X.XX.X-Y.x86_64.rpm
```
Replace X.XX.X-Y with the application version.

## SUSE/OpenSUSE Linux
Import qt6-omodscan.pubkey to rpm repository:
```bash
sudo rpm --import qt6-omodscan.pubkey
```
Install the RPM package using Zypper:
```bash
sudo dnf install ./qt6-omodscan_X.XX.X-Y.x86_64.rpm
```
Replace X.XX.X-Y with the application version.

## Flatpak
Run the followning command:
```bash
flatpak install --user io.github.sanny32.omodscan.flatpak
```

# MIT License
Copyright 2025 Alexandr Ananev [mail@ananev.org]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

