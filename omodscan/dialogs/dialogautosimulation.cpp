#include <float.h>
#include <QPushButton>
#include "dialogautosimulation.h"
#include "ui_dialogautosimulation.h"

///
/// \brief DialogAutoSimulation::DialogAutoSimulation
/// \param params
/// \param parent
///
DialogAutoSimulation::DialogAutoSimulation(DataDisplayMode mode, ModbusSimulationParams& params, QWidget *parent)
    : QFixedSizeDialog(parent)
    , ui(new Ui::DialogAutoSimulation)
    ,_params(params)
    ,_displayMode(mode)
{
    ui->setupUi(this);
    ui->checkBoxEnabled->setChecked(_params.Mode != SimulationMode::No);
    ui->comboBoxSimulationType->setup(QModbusDataUnit::HoldingRegisters);

    if(_params.Mode != SimulationMode::No)
        ui->comboBoxSimulationType->setCurrentSimulationMode(_params.Mode);
    else
        ui->comboBoxSimulationType->setCurrentIndex(0);

    ui->lineEditInterval->setInputRange(1, 60000);
    ui->lineEditInterval->setValue(_params.Interval);

    switch(_displayMode)
    {
        case DataDisplayMode::Binary:
        break;

        case DataDisplayMode::Decimal:
            ui->lineEditStepValue->setInputRange(1, USHRT_MAX - 1);
            ui->lineEditLowLimit->setInputRange(0, USHRT_MAX);
            ui->lineEditHighLimit->setInputRange(0, USHRT_MAX);
        break;

        case DataDisplayMode::Integer:
            ui->lineEditStepValue->setInputRange(1, SHRT_MAX - 1);
            ui->lineEditLowLimit->setInputRange(SHRT_MIN, SHRT_MAX);
            ui->lineEditHighLimit->setInputRange(SHRT_MIN, SHRT_MAX);
        break;

        case DataDisplayMode::LongInteger:
        case DataDisplayMode::SwappedLI:
            ui->lineEditStepValue->setInputRange(1, INT_MAX - 1);
            ui->lineEditLowLimit->setInputRange(INT_MIN, INT_MAX);
            ui->lineEditHighLimit->setInputRange(INT_MIN, INT_MAX);
        break;

        case DataDisplayMode::UnsignedLongInteger:
        case DataDisplayMode::SwappedUnsignedLI:
            ui->lineEditStepValue->setInputRange(1U, UINT_MAX - 1);
            ui->lineEditStepValue->setInputMode(NumericLineEdit::UnsignedMode);
            ui->lineEditLowLimit->setInputRange(0U, UINT_MAX);
            ui->lineEditLowLimit->setInputMode(NumericLineEdit::UnsignedMode);
            ui->lineEditHighLimit->setInputRange(0U, UINT_MAX);
            ui->lineEditHighLimit->setInputMode(NumericLineEdit::UnsignedMode);
        break;

        case DataDisplayMode::Hex:
            ui->lineEditStepValue->setInputRange(1, USHRT_MAX - 1);
            ui->lineEditLowLimit->setInputRange(0, USHRT_MAX);
            ui->lineEditHighLimit->setInputRange(0, USHRT_MAX);
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditStepValue->setInputRange((float)1, FLT_MAX - 1);
            ui->lineEditStepValue->setInputMode(NumericLineEdit::FloatMode);
            ui->lineEditLowLimit->setInputRange(-FLT_MAX, FLT_MAX);
            ui->lineEditHighLimit->setInputRange(-FLT_MAX, FLT_MAX);
            ui->lineEditLowLimit->setInputMode(NumericLineEdit::FloatMode);
            ui->lineEditHighLimit->setInputMode(NumericLineEdit::FloatMode);
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditStepValue->setInputRange(1.0, DBL_MAX - 1);
            ui->lineEditStepValue->setInputMode(NumericLineEdit::DoubleMode);
            ui->lineEditLowLimit->setInputRange(-DBL_MAX, DBL_MAX);
            ui->lineEditHighLimit->setInputRange(-DBL_MAX, DBL_MAX);
            ui->lineEditLowLimit->setInputMode(NumericLineEdit::DoubleMode);
            ui->lineEditHighLimit->setInputMode(NumericLineEdit::DoubleMode);
        break;
    }

    updateLimits();
    on_checkBoxEnabled_toggled();
}

///
/// \brief DialogAutoSimulation::~DialogAutoSimulation
///
DialogAutoSimulation::~DialogAutoSimulation()
{
    delete ui;
}

///
/// \brief DialogAutoSimulation::accept
///
void DialogAutoSimulation::accept()
{
    if(ui->checkBoxEnabled->isChecked())
    {
        _params.Mode = ui->comboBoxSimulationType->currentSimulationMode();
        _params.Interval = ui->lineEditInterval->value<int>();

        switch(_params.Mode)
        {
            case SimulationMode::Random:
                _params.RandomParams.Range = QRange<double>(ui->lineEditLowLimit->value<double>(),
                                                            ui->lineEditHighLimit->value<double>());
            break;

            case SimulationMode::Increment:
                _params.IncrementParams.Step = ui->lineEditStepValue->value<double>();
                _params.IncrementParams.Range = QRange<double>(ui->lineEditLowLimit->value<double>(),
                                                               ui->lineEditHighLimit->value<double>());
            break;

            case SimulationMode::Decrement:
                _params.DecrementParams.Step = ui->lineEditStepValue->value<double>();
                _params.DecrementParams.Range = QRange<double>(ui->lineEditLowLimit->value<double>(),
                                                               ui->lineEditHighLimit->value<double>());
            break;

            default:
            break;
        }

    }
    else
    {
        _params.Mode = SimulationMode::No;
    }

    QFixedSizeDialog::accept();
}

///
/// \brief DialogAutoSimulation::on_checkBoxEnabled_toggled
///
void DialogAutoSimulation::on_checkBoxEnabled_toggled()
{
    const bool enabled = ui->checkBoxEnabled->isChecked();
    const auto mode = ui->comboBoxSimulationType->currentSimulationMode();
    ui->labelSimulationType->setEnabled(enabled);
    ui->comboBoxSimulationType->setEnabled(enabled);
    ui->labelInterval->setEnabled(enabled);
    ui->lineEditInterval->setEnabled(enabled);
    ui->labelStepValue->setEnabled(enabled && mode != SimulationMode::Random);
    ui->lineEditStepValue->setEnabled(enabled && mode != SimulationMode::Random);
    ui->groupBoxSimulatioRange->setEnabled(enabled);
}

///
/// \brief DialogAutoSimulation::on_comboBoxSimulationType_currentIndexChanged
/// \param idx
///
void DialogAutoSimulation::on_comboBoxSimulationType_currentIndexChanged(int idx)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(idx != -1);
    updateLimits();
}

///
/// \brief DialogAutoSimulation::on_lineEditLowLimit_valueChanged
///
void DialogAutoSimulation::on_lineEditLowLimit_valueChanged(const QVariant&)
{
    const auto lolim = ui->lineEditLowLimit->value<double>();
    const auto hilim = ui->lineEditHighLimit->value<double>();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(lolim < hilim);
}

///
/// \brief DialogAutoSimulation::on_lineEditHighLimit_valueChanged
///
void DialogAutoSimulation::on_lineEditHighLimit_valueChanged(const QVariant&)
{
    const auto lolim = ui->lineEditLowLimit->value<double>();
    const auto hilim = ui->lineEditHighLimit->value<double>();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(lolim < hilim);
}

///
/// \brief DialogAutoSimulation::updateLimits
///
void DialogAutoSimulation::updateLimits()
{
    switch(ui->comboBoxSimulationType->currentSimulationMode())
    {
        case SimulationMode::Random:
            ui->labelStepValue->setEnabled(false);
            ui->lineEditStepValue->setEnabled(false);
            ui->lineEditLowLimit->setValue(_params.RandomParams.Range.from());
            ui->lineEditHighLimit->setValue(_params.RandomParams.Range.to());
        break;

        case SimulationMode::Increment:
            ui->labelStepValue->setEnabled(true);
            ui->lineEditStepValue->setEnabled(true);
            ui->lineEditStepValue->setValue(_params.IncrementParams.Step);
            ui->lineEditLowLimit->setValue(_params.IncrementParams.Range.from());
            ui->lineEditHighLimit->setValue(_params.IncrementParams.Range.to());
        break;

        case SimulationMode::Decrement:
            ui->labelStepValue->setEnabled(true);
            ui->lineEditStepValue->setEnabled(true);
            ui->lineEditStepValue->setValue(_params.DecrementParams.Step);
            ui->lineEditLowLimit->setValue(_params.DecrementParams.Range.from());
            ui->lineEditHighLimit->setValue(_params.DecrementParams.Range.to());
        break;

        default:
        break;
    }
}
