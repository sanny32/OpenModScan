#include "modbusfunction.h"
#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    :QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
}

///
/// \brief FunctionCodeComboBox::currentFunctionCode
/// \return
///
QModbusPdu::FunctionCode FunctionCodeComboBox::currentFunctionCode() const
{
    return currentData().value<QModbusPdu::FunctionCode>();
}

///
/// \brief FunctionCodeComboBox::setCurrentFunctionCode
/// \param pointType
///
void FunctionCodeComboBox::setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode)
{
    const auto idx = findData(funcCode);
    setCurrentIndex(idx);
}

///
/// \brief FunctionCodeComboBox::addItem
/// \param funcCode
///
void FunctionCodeComboBox::addItem(QModbusPdu::FunctionCode funcCode)
{
    ModbusFunction func(funcCode);
    const auto code = QString("%1").arg(QString::number(func, 10), 2, '0');
    QComboBox::addItem(QString("%1: %2").arg(code, func), funcCode);
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    emit functionCodeChanged(itemData(index).value<QModbusPdu::FunctionCode>());
}
