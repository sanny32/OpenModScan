#include "modbuslimits.h"
#include "dialogwritecoilregister.h"
#include "ui_dialogwritecoilregister.h"

///
/// \brief DialogWriteCoilRegister::DialogWriteCoilRegister
/// \param params
/// \param parent
///
DialogWriteCoilRegister::DialogWriteCoilRegister(const ModbusWriteParams& params, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWriteCoilRegister)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());

    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);
    ui->radioButtonOn->setChecked(params.Value.toBool());
    ui->radioButtonOff->setChecked(!params.Value.toBool());
}

///
/// \brief DialogWriteCoilRegister::~DialogWriteCoilRegister
///
DialogWriteCoilRegister::~DialogWriteCoilRegister()
{
    delete ui;
}

///
/// \brief DialogWriteCoilRegister::writeParams
/// \return
///
ModbusWriteParams DialogWriteCoilRegister::writeParams() const
{
    const quint32 node = ui->lineEditNode->value();
    const quint32 addr = ui->lineEditAddress->value();
    const bool value = ui->radioButtonOn->isChecked();
    return {node, addr, value };
}
