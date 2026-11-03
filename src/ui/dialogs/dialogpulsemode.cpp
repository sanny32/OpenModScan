#include "dialogpulsemode.h"
#include "ui_dialogpulsemode.h"

///
/// \brief DialogPulseMode::DialogPulseMode
/// \param parent
///
DialogPulseMode::DialogPulseMode(PulseParams& params, QWidget *parent)
    : QFixedSizeDialog(parent)
    , ui(new Ui::DialogPulseMode)
    ,_params(params)
{
    ui->setupUi(this);
    ui->comboBoxRestore->setItemData(0, PulseParams::Previous);
    ui->comboBoxRestore->setItemData(1, PulseParams::Zero);

    ui->comboBoxDuration->setMinimum(100);
    ui->comboBoxDuration->setMaximum(100000);
    ui->comboBoxDuration->setCurrentValue(_params.Duration);
    ui->comboBoxRestore->setCurrentIndex(_params.Restore == PulseParams::Previous ? 0 : 1);
}

///
/// \brief DialogPulseMode::~DialogPulseMode
///
DialogPulseMode::~DialogPulseMode()
{
    delete ui;
}

///
/// \brief DialogPulseMode::accept
///
void DialogPulseMode::accept()
{
    _params.Duration = ui->comboBoxDuration->currentValue();
    _params.Restore = ui->comboBoxRestore->currentData().value<PulseParams::RestoreMode>();

    QFixedSizeDialog::accept();
}