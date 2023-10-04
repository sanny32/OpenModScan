#include "modbusfunction.h"
#include "formatutils.h"
#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    : QComboBox(parent)
    ,_dataDisplayMode(DataDisplayMode::Decimal)
{
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FunctionCodeComboBox::on_currentIndexChanged);
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
/// \brief FunctionCodeComboBox::dataDisplayMode
/// \return
///
DataDisplayMode FunctionCodeComboBox::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief FunctionCodeComboBox::setDataDisplayMode
/// \param mode
///
void FunctionCodeComboBox::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
}

///
/// \brief FunctionCodeComboBox::addItem
/// \param funcCode
///
void FunctionCodeComboBox::addItem(QModbusPdu::FunctionCode funcCode)
{
    const auto code = formatFuncCode(_dataDisplayMode, funcCode);
    QComboBox::addItem(QString("%1: %2").arg(code, ModbusFunction(funcCode)), funcCode);
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    emit functionCodeChanged(itemData(index).value<QModbusPdu::FunctionCode>());
}
