#include "pointtypecombobox.h"


///
/// \brief PointTypeComboBox::PointTypeComboBox
/// \param parent
///
PointTypeComboBox::PointTypeComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem("01: COIL STATUS", QModbusDataUnit::Coils);
    addItem("02: INPUT STATUS", QModbusDataUnit::DiscreteInputs);
    addItem("03: HOLDING REGISTER", QModbusDataUnit::HoldingRegisters);
    addItem("04: INPUT REGISTER", QModbusDataUnit::InputRegisters);
}

///
/// \brief PointTypeComboBox::currentPointType
/// \return
///
QModbusDataUnit::RegisterType PointTypeComboBox::currentPointType() const
{
    return currentData().value<QModbusDataUnit::RegisterType>();
}

///
/// \brief PointTypeComboBox::setCurrentPointType
/// \param pointType
///
void PointTypeComboBox::setCurrentPointType(QModbusDataUnit::RegisterType pointType)
{
    const auto idx = findData(pointType);
    setCurrentIndex(idx);
}
