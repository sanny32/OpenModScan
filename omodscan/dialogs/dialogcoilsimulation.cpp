#include <QPushButton>
#include "dialogcoilsimulation.h"
#include "ui_dialogcoilsimulation.h"

///
/// \brief DialogCoilSimulation::DialogCoilSimulation
/// \param params
/// \param parent
///
DialogCoilSimulation::DialogCoilSimulation(ModbusSimulationParams& params, QWidget *parent)
    : QFixedSizeDialog(parent)
    , ui(new Ui::DialogCoilSimulation)
    ,_params(params)
{
    ui->setupUi(this);
    ui->checkBoxEnabled->setChecked(_params.Mode != SimulationMode::No);
    ui->comboBoxSimulationType->setup(QModbusDataUnit::Coils);

    if(_params.Mode != SimulationMode::No)
        ui->comboBoxSimulationType->setCurrentSimulationMode(_params.Mode);
    else
        ui->comboBoxSimulationType->setCurrentIndex(0);

    ui->lineEditInterval->setInputRange(1, 60000);
    ui->lineEditInterval->setValue(_params.Interval);
    on_checkBoxEnabled_toggled();
}

///
/// \brief DialogCoilSimulation::~DialogCoilSimulation
///
DialogCoilSimulation::~DialogCoilSimulation()
{
    delete ui;
}

///
/// \brief DialogCoilSimulation::accept
///
void DialogCoilSimulation::accept()
{
    if(ui->checkBoxEnabled->isChecked())
    {
        _params.Mode = ui->comboBoxSimulationType->currentSimulationMode();
        _params.Interval = ui->lineEditInterval->value<int>();

        if(_params.Mode == SimulationMode::Random)
            _params.RandomParams.Range = QRange<double>(0, 1);
    }
    else
    {
        _params.Mode = SimulationMode::No;
    }
    QFixedSizeDialog::accept();
}

///
/// \brief DialogCoilSimulation::on_checkBoxEnabled_toggled
///
void DialogCoilSimulation::on_checkBoxEnabled_toggled()
{
    const bool enabled = ui->checkBoxEnabled->isChecked();
    ui->labelSimulationType->setEnabled(enabled);
    ui->comboBoxSimulationType->setEnabled(enabled);
    ui->labelInterval->setEnabled(enabled);
    ui->lineEditInterval->setEnabled(enabled);
}

///
/// \brief DialogCoilSimulation::on_comboBoxSimulationType_currentIndexChanged
/// \param idx
///
void DialogCoilSimulation::on_comboBoxSimulationType_currentIndexChanged(int idx)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(idx != -1);
}
