#include <QBitArray>
#include "modbuslimits.h"
#include "dialogwriteholdingregisterbits.h"
#include "ui_dialogwriteholdingregisterbits.h"

///
/// \brief DialogWriteHoldingRegisterBits::DialogWriteHoldingRegisterBits
/// \param params
/// \param hexAddress
/// \param parent
///
DialogWriteHoldingRegisterBits::DialogWriteHoldingRegisterBits(ModbusWriteParams& params, bool hexAddress, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegisterBits)
    ,_writeParams(params)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);

    const quint16 value = params.Value.toUInt();
    for (int i = 0; i < 16; i++)
    {
        auto ctrl = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(ctrl) ctrl->setChecked(value >> i & 1);
    }
    ui->buttonBox->setFocus();
}

///
/// \brief DialogWriteHoldingRegisterBits::~DialogWriteHoldingRegisterBits
///
DialogWriteHoldingRegisterBits::~DialogWriteHoldingRegisterBits()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegisterBits::accept
///
void DialogWriteHoldingRegisterBits::accept()
{
    quint16 value = 0;
    for (int i = 0; i < 16; i++)
    {
        auto ctrl = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(ctrl) value |= ctrl->isChecked() << i;
    }
    _writeParams.Value = value;
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Node = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}
