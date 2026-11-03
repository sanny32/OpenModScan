#include "dialogprotocolselections.h"
#include "ui_dialogprotocolselections.h"

///
/// \brief DialogProtocolSelections::DialogProtocolSelections
/// \param mps
/// \param parent
///
DialogProtocolSelections::DialogProtocolSelections(ModbusProtocolSelections& mps, QWidget *parent) :
     QFixedSizeDialog(parent)
    , ui(new Ui::DialogProtocolSelections)
    ,_protocolSelections(mps)
{
    ui->setupUi(this);
    ui->lineEditTimeout->setInputRange(10, 300000);
    ui->lineEditDelay->setInputRange(0, 300000);
    ui->lineEditTimeout->setValue(mps.SlaveResponseTimeOut);
    ui->spinBoxRetries->setValue(mps.NumberOfRetries);
    ui->lineEditDelay->setValue(mps.InterFrameDelay);
    ui->checkBoxForce->setChecked(mps.ForceModbus15And16Func);
    ui->buttonBox->setFocus();
}

///
/// \brief DialogProtocolSelections::~DialogProtocolSelections
///
DialogProtocolSelections::~DialogProtocolSelections()
{
    delete ui;
}

///
/// \brief DialogProtocolSelections::accept
///
void DialogProtocolSelections::accept()
{
    _protocolSelections.SlaveResponseTimeOut = ui->lineEditTimeout->value<int>();
    _protocolSelections.NumberOfRetries = ui->spinBoxRetries->value();
    _protocolSelections.InterFrameDelay = ui->lineEditDelay->value<int>();
    _protocolSelections.ForceModbus15And16Func = ui->checkBoxForce->isChecked();

    QFixedSizeDialog::accept();
}
