# Open ModScan
Open ModScan is a free implimentation of modbus master (client) utility for modbus-tcp and modbus-rtu protocols.

![image](https://github.com/sanny32/OpenModScan/assets/13627951/aa912ece-4b76-44b4-9523-5b0b0156a64b)


![image](https://github.com/sanny32/OpenModScan/assets/13627951/77bee5d8-09a8-4845-8d64-02b7bf3cf592)


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

Modbus Logging

![image](https://github.com/sanny32/OpenModScan/assets/13627951/de14b977-08ba-460c-814c-7affd0d88f91)

    
## Extended Featues
- Modbus Address Scan
  
  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/8989fbde-09f1-435c-a9a7-31e27a0ec576)

- Modbus Scanner (supports both Modbus RTU and Modbus TCP scanning)
  
  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/056de3b1-026b-498b-914c-4d1e565067bb)

- Modbus Message Parser

  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/86a82340-015e-4ee9-a483-b5ab83527cc1)

- Modbus User Message
  
  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/5ee959a2-9c5c-4ffe-8056-d6205e3b5aa3)

## Building
  Now building is available with Qt/qmake (version 5.15 and above) or Qt Creator. Supports both OS Microsoft Windows and Linux.
  
## MIT License
Copyright 2023 Alexandr Ananev [mail@ananev.org]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

