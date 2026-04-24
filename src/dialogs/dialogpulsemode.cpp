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
}

///
/// \brief DialogPulseMode::~DialogPulseMode
///
DialogPulseMode::~DialogPulseMode()
{
    delete ui;
}
