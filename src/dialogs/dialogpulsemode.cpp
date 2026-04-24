#include "dialogpulsemode.h"
#include "ui_dialogpulsemode.h"

DialogPulseMode::DialogPulseMode(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogPulseMode)
{
    ui->setupUi(this);
}

DialogPulseMode::~DialogPulseMode()
{
    delete ui;
}
