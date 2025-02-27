# Open ModScan
Open ModScan is a free implimentation of modbus master (client) utility for modbus-tcp and modbus-rtu protocols.

![image](https://github.com/user-attachments/assets/aaa2b5c8-9f47-4e17-803d-2af7d24fb0c7)


![image](https://github.com/user-attachments/assets/2bfe852f-1cc2-433d-8917-70815228a1c3)


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

![image](https://github.com/user-attachments/assets/8d24e149-e6db-47e1-a3d2-b2d5ed207ed4)

    
## Extended Featues
- Modbus Address Scan
  
 ![image](https://github.com/user-attachments/assets/798ed74a-c5fc-413f-a173-e223c9c7d3f3)

- Modbus Scanner (supports both Modbus RTU and Modbus TCP scanning)
  
  ![image](https://github.com/user-attachments/assets/17d5f43d-c341-455d-a9b8-67db50a35699)

- Modbus Message Parser

  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/4f05f38e-d739-4c49-8bc3-f12e7b74d8ab)

- Modbus User Message
  
  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/1aba6329-873c-4ff2-8db8-939245a50722)

## Building
  Building is available with cmake (with installed Qt version 5.15 and above) or Qt Creator. Supports both OS Microsoft Windows and Linux.
  
## MIT License
Copyright 2024 Alexandr Ananev [mail@ananev.org]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

