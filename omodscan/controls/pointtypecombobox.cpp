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

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
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

///
/// \brief PointTypeComboBox::on_currentIndexChanged
/// \param index
///
void PointTypeComboBox::on_currentIndexChanged(int index)
{
    emit pointTypeChanged(itemData(index).value<QModbusDataUnit::RegisterType>());
}
