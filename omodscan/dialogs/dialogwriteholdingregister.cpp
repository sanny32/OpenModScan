#include <float.h>
#include "modbuslimits.h"
#include "dialogautosimulation.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param params
/// \param simParams
/// \param mode
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(ModbusWriteParams& params, ModbusSimulationParams& simParams, DataDisplayMode mode, QWidget* parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
    ,_writeParams(params)
    ,_simParams(simParams)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);

    switch(mode)
    {
        case DataDisplayMode::Binary:
        break;

        case DataDisplayMode::Decimal:
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::Integer:
            ui->lineEditValue->setInputRange(SHRT_MIN, SHRT_MAX);
            ui->lineEditValue->setValue(params.Value.toInt());
        break;

        case DataDisplayMode::Hex:
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
            ui->labelValue->setText(tr("Value, (HEX): "));
            ui->lineEditValue->setPaddingZeroes(true);
            ui->lineEditValue->setInputMode(NumericLineEdit::HexMode);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditValue->setInputRange(-FLT_MAX, FLT_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::FloatMode);
            ui->lineEditValue->setValue(params.Value.toFloat());
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditValue->setInputRange(-DBL_MAX, DBL_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::DoubleMode);
            ui->lineEditValue->setValue(params.Value.toDouble());
        break;

        case DataDisplayMode::LongInteger:
        case DataDisplayMode::SwappedLI:
            ui->lineEditValue->setInputRange(INT_MIN, INT_MAX);
            ui->lineEditValue->setValue(params.Value.toInt());
        break;

        case DataDisplayMode::UnsignedLongInteger:
        case DataDisplayMode::SwappedUnsignedLI:
            ui->lineEditValue->setInputRange(0U, UINT_MAX);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;
    }
    ui->buttonBox->setFocus();
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

///
/// \brief DialogWriteHoldingRegister::on_pushButtonSimulation_clicked
///
void DialogWriteHoldingRegister::on_pushButtonSimulation_clicked()
{
    DialogAutoSimulation dlg(_writeParams.DisplayMode, _simParams, this);
    if(dlg.exec() == QDialog::Accepted) done(2);
}
