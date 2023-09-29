#include "modbuslimits.h"
#include "displaydefinition.h"
#include "dialogdisplaydefinition.h"
#include "ui_dialogdisplaydefinition.h"

///
/// \brief DialogDisplayDefinition::DialogDisplayDefinition
/// \param parent
///
DialogDisplayDefinition::DialogDisplayDefinition(FormModSca* parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogDisplayDefinition)
{
    ui->setupUi(this);
    ui->lineEditScanRate->setInputRange(20, 36000000);
    ui->lineEditPointAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditLogLimit->setInputRange(4, 1000);

    const auto dd = parent->displayDefinition();
    ui->lineEditScanRate->setValue(dd.ScanRate);
    ui->lineEditPointAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(dd.Length);
    ui->lineEditLogLimit->setValue(dd.LogViewLimit);
    ui->comboBoxPointType->setCurrentPointType(dd.PointType);

    ui->buttonBox->setFocus();
}

///
/// \brief DialogDisplayDefinition::~DialogDisplayDefinition
///
DialogDisplayDefinition::~DialogDisplayDefinition()
{
    delete ui;
}

///
/// \brief DialogDisplayDefinition::accept
///
void DialogDisplayDefinition::accept()
{
    DisplayDefinition dd;
    dd.DeviceId = ui->lineEditSlaveAddress->value<int>();
    dd.PointAddress = ui->lineEditPointAddress->value<int>();
    dd.PointType = ui->comboBoxPointType->currentPointType();
    dd.Length = ui->lineEditLength->value<int>();
    dd.ScanRate = ui->lineEditScanRate->value<int>();
    dd.LogViewLimit = ui->lineEditLogLimit->value<int>();
    ((FormModSca*)parentWidget())->setDisplayDefinition(dd);

    QFixedSizeDialog::accept();
}
