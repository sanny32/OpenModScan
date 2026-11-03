#ifndef MODBUSDEVICE_H
#define MODBUSDEVICE_H

#include <QMetaType>

namespace ModbusDevice
{
    Q_NAMESPACE
    enum Error {
        NoError,
        ReadError,
        WriteError,
        ConnectionError,
        ConfigurationError,
        TimeoutError,
        ProtocolError,
        ReplyAbortedError,
        UnknownError,
        InvalidResponseError
    };
    Q_ENUM_NS(Error)

    enum IntermediateError
    {
        ResponseCrcError,
        ResponseRequestMismatch
    };
    Q_ENUM_NS(IntermediateError)

    Q_NAMESPACE
    enum State {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
        ClosingState
    };
    Q_ENUM_NS(State)

    enum ConnectionParameter {
        SerialPortNameParameter,
        SerialParityParameter,
        SerialBaudRateParameter,
        SerialDataBitsParameter,
        SerialStopBitsParameter,

        NetworkPortParameter,
        NetworkAddressParameter
    };
    Q_ENUM_NS(ConnectionParameter)
}

Q_DECLARE_METATYPE(ModbusDevice::State)
Q_DECLARE_METATYPE(ModbusDevice::Error)
Q_DECLARE_METATYPE(ModbusDevice::IntermediateError)
Q_DECLARE_METATYPE(ModbusDevice::ConnectionParameter)

#endif // MODBUSDEVICE_H
