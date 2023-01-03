#include <float.h>
#include "modbuslimits.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param params
/// \param mode
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(ModbusWriteParams& params, DataDisplayMode mode, QWidget* parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
    ,_writeParams(params)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditValue->setInputRange(0, 65535);
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);

    switch(mode)
    {
        case DataDisplayMode::Binary:
        break;

        case DataDisplayMode::Decimal:
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::Integer:
            ui->lineEditValue->setValue(params.Value.toInt());
        break;

        case DataDisplayMode::Hex:
            ui->labelValue->setText("Value, (HEX): ");
            ui->lineEditValue->setPaddingZeroes(true);
            ui->lineEditValue->setInputMode(NumericLineEdit::HexMode);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditValue->setInputRange(FLT_MIN, FLT_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::RealMode);
            ui->lineEditValue->setValue(params.Value.toFloat());
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditValue->setInputRange(DBL_MIN, DBL_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::RealMode);
            ui->lineEditValue->setValue(params.Value.toDouble());
        break;
    }
}

///
/// \brief DialogWriteHoldingRegister::~DialogWriteHoldingRegister
///
DialogWriteHoldingRegister::~DialogWriteHoldingRegister()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegister::accept
///
void DialogWriteHoldingRegister::accept()
{
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Value = ui->lineEditValue->value<QVariant>();
    _writeParams.Node = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}
