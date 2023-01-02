#include <QDebug>
#include <QMessageBox>
#include "modbuslimits.h"
#include "modbusexception.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param params
/// \param mode
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(const ModbusWriteParams& params, DisplayMode mode, QWidget* parent) :
      QDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());

    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditValue->setInputRange(0, 65535);
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);
    ui->lineEditValue->setValue(params.Value.toInt());
}

///
/// \brief DialogWriteHoldingRegister::~DialogWriteHoldingRegister
///
DialogWriteHoldingRegister::~DialogWriteHoldingRegister()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegister::writeParams
/// \return
///
ModbusWriteParams DialogWriteHoldingRegister::writeParams() const
{
    const quint32 node = ui->lineEditNode->value();
    const quint32 addr = ui->lineEditAddress->value();
    const auto value = ui->lineEditValue->value();
    return {node, addr, value };
}
