# Open ModScan
Open ModScan is a free implimentation of modbus master (client) utility for modbus-tcp and and modbus-rtu protocols.

![изображение](https://user-images.githubusercontent.com/13627951/211140780-cf5f1b7d-6709-429e-b4e7-0d85806f1955.png)

![изображение](https://user-images.githubusercontent.com/13627951/211140886-32905d1c-559f-4a6b-aaaf-9396962c3a28.png)

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
    0x17 - Read Write Multiple Registers

## Building
  Now building is available with Qt/qmake or Qt Creator. Supports both OS Microsoft Windows and Linux.
  
## License
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
