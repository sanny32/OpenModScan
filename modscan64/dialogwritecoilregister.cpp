#include "modbuslimits.h"
#include "dialogwritecoilregister.h"
#include "ui_dialogwritecoilregister.h"

///
/// \brief DialogWriteCoilRegister::DialogWriteCoilRegister
/// \param params
/// \param parent
///
DialogWriteCoilRegister::DialogWriteCoilRegister(ModbusWriteParams& params, QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogWriteCoilRegister)
    ,_writeParams(params)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);
    ui->radioButtonOn->setChecked(params.Value.toBool());
    ui->radioButtonOff->setChecked(!params.Value.toBool());
    ui->buttonBox->setFocus();
}

///
/// \brief DialogWriteCoilRegister::~DialogWriteCoilRegister
///
DialogWriteCoilRegister::~DialogWriteCoilRegister()
{
    delete ui;
}

///
/// \brief DialogWriteCoilRegister::accept
///
void DialogWriteCoilRegister::accept()
{
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Value = ui->radioButtonOn->isChecked();
    _writeParams.Node = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}
