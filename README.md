# Open ModScan
Open ModScan is a free implimentation of modbus master (client) utility for modbus-tcp and modbus-rtu protocols.

![image](https://user-images.githubusercontent.com/13627951/227959348-7db5a333-8443-4824-873e-3d87d3ce6f8a.png)


![image](https://user-images.githubusercontent.com/13627951/227959984-7adbf285-fd1d-4198-8ff9-386a4eea0a87.png)


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
    
## Extended Featues
  Modbus Address Scan
  
  ![image](https://user-images.githubusercontent.com/13627951/226310086-4160c8c7-503a-48c0-bdf8-f5bb970592d3.png)

  Modbus Scanner (supports both Modbus RTU and Modbus TCP scanning)
  
  
  ![image](https://github.com/sanny32/OpenModScan/assets/13627951/056de3b1-026b-498b-914c-4d1e565067bb)



## Building
  Now building is available with Qt/qmake (version 5.15 and above) or Qt Creator. Supports both OS Microsoft Windows and Linux.
  
## MIT License
Copyright 2023 Alexandr Ananev [mail@ananev.org]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

