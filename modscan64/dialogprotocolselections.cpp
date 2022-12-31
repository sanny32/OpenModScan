#include "dialogprotocolselections.h"
#include "ui_dialogprotocolselections.h"

///
/// \brief DialogProtocolSelections::DialogProtocolSelections
/// \param mps
/// \param parent
///
DialogProtocolSelections::DialogProtocolSelections(ModbusProtocolSelections& mps, QWidget *parent) :
     QDialog(parent)
    , ui(new Ui::DialogProtocolSelections)
    ,_protocolSelections(mps)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());

    ui->lineEditTimeout->setInputRange(10, 300000);
    ui->lineEditTimeout->setInputRange(0, 300000);

    ui->lineEditTimeout->setValue(mps.SlaveResponseTimeOut);
    ui->lineEditDelay->setValue(mps.DelayBetweenPolls);
    ui->checkBoxForce->setChecked(mps.ForceModbus15And16Func);
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
    _protocolSelections.SlaveResponseTimeOut = ui->lineEditTimeout->value();
    _protocolSelections.DelayBetweenPolls = ui->lineEditDelay->value();
    _protocolSelections.ForceModbus15And16Func = ui->checkBoxForce->isChecked();

    QDialog::accept();
}
