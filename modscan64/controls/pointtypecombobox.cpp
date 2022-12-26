#include "pointtypecombobox.h"

///
/// \brief PointTypeComboBox::PointTypeComboBox
/// \param parent
///
PointTypeComboBox::PointTypeComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem("01: COIL STATUS", 1);
    addItem("02: INPUT STATUS", 2);
    addItem("03: HOLDING REGISTER", 3);
    addItem("04: INPUT REGISTER", 4);
}

///
/// \brief PointTypeComboBox::currentPointType
/// \return
///
uint PointTypeComboBox::currentPointType() const
{
    return currentData().toUInt();
}

///
/// \brief PointTypeComboBox::setCurrentPointType
/// \param pointType
///
void PointTypeComboBox::setCurrentPointType(uint pointType)
{
    const auto idx = findData(pointType);
    setCurrentIndex(idx);
}
