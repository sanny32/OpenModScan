#ifndef MODBUSDATAUNIT_H
#define MODBUSDATAUNIT_H

#include <QModbusDataUnit>

///
/// \brief The ModbusDataUnit class
///
class ModbusDataUnit : public QModbusDataUnit
{
public:
    ModbusDataUnit() = default;
    explicit ModbusDataUnit(RegisterType type);
    explicit ModbusDataUnit(RegisterType type, int newStartAddress, quint16 newValueCount);
    explicit ModbusDataUnit(RegisterType type, int newStartAddress, const QList<quint16> &newValues);

    bool hasValue(qsizetype index) const;
    void setValue(qsizetype index, quint16 newValue);

private:
    QVector<bool> _hasValues;
};

#endif // MODBUSDATAUNIT_H
