#include "modbusdataunit.h"

///
/// \brief ModbusDataUnit::ModbusDataUnit
/// \param type
///
ModbusDataUnit::ModbusDataUnit(RegisterType type)
    : QModbusDataUnit(type)
{
}

///
/// \brief ModbusDataUnit::ModbusDataUnit
/// \param type
/// \param newStartAddress
/// \param newValueCount
///
ModbusDataUnit::ModbusDataUnit(RegisterType type, int newStartAddress, quint16 newValueCount)
    : QModbusDataUnit(type, newStartAddress, newValueCount)
{
    _hasValues.resize(newValueCount);
}

///
/// \brief ModbusDataUnit::hasValue
/// \param index
/// \return
///
bool ModbusDataUnit::hasValue(qsizetype index) const
{
    if(index < 0 || index >= _hasValues.count()) return false;
    return _hasValues.at(index);
}

///
/// \brief ModbusDataUnit::setValue
/// \param index
/// \param newValue
///
void ModbusDataUnit::setValue(qsizetype index, quint16 newValue)
{
    QModbusDataUnit::setValue(index, newValue);
    if(index >=0 && index < _hasValues.count()) _hasValues[index] = true;
}
