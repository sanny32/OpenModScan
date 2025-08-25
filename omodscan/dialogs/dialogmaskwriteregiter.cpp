#include "modbuslimits.h"
#include "dialogmaskwriteregiter.h"
#include "ui_dialogmaskwriteregiter.h"

///
/// \brief DialogMaskWriteRegiter::DialogMaskWriteRegiter
/// \param parent
///
DialogMaskWriteRegiter::DialogMaskWriteRegiter(ModbusMaskWriteParams& params, bool hexAddress, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogMaskWriteRegiter)
    ,_writeParams(params)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);

    for (int i = 0; i < 16; i++)
    {
        auto ctrlAnd = findChild<QCheckBox*>(QString("checkBoxAnd%1").arg(i));
        if(ctrlAnd) ctrlAnd->setChecked(params.AndMask >> i & 1);

        auto ctrlOr = findChild<QCheckBox*>(QString("checkBoxOr%1").arg(i));
        if(ctrlOr) ctrlOr->setChecked(params.OrMask >> i & 1);
    }
    ui->buttonBox->setFocus();
}

///
/// \brief DialogMaskWriteRegiter::~DialogMaskWriteRegiter
///
DialogMaskWriteRegiter::~DialogMaskWriteRegiter()
{
    delete ui;
}

///
/// \brief DialogMaskWriteRegiter::accept
///
void DialogMaskWriteRegiter::accept()
{
    quint16 maskAnd = 0, maskOr = 0;
    for (int i = 0; i < 16; i++)
    {
        auto ctrlAnd = findChild<QCheckBox*>(QString("checkBoxAnd%1").arg(i));
        if(ctrlAnd) maskAnd |= ctrlAnd->isChecked() << i;

        auto ctrlOr = findChild<QCheckBox*>(QString("checkBoxOr%1").arg(i));
        if(ctrlOr) maskOr |= ctrlOr->isChecked() << i;

    }
    _writeParams.AndMask = maskAnd;
    _writeParams.OrMask = maskOr;
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Node = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}
