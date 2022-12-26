#include "displaydefinition.h"
#include "dialogdisplaydefinition.h"
#include "ui_dialogdisplaydefinition.h"

///
/// \brief DialogDisplayDefinition::DialogDisplayDefinition
/// \param parent
///
DialogDisplayDefinition::DialogDisplayDefinition(FormModSca* parent) :
    QDialog(parent),
    ui(new Ui::DialogDisplayDefinition)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());

    ui->lineEditScanRate->setInputRange(5, 10000);
    ui->lineEditPointAddress->setInputRange(1, 65534);
    ui->lineEditLength->setInputRange(1, 128);
    ui->lineEditSlaveAddress->setInputRange(1, 255);

    const auto dd = parent->displayDefinition();
    ui->lineEditScanRate->setValue(dd.ScanRate);
    ui->lineEditPointAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxPointType->setCurrentPointType(dd.PointType);
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
    dd.DeviceId = ui->lineEditSlaveAddress->value();
    dd.PointAddress = ui->lineEditPointAddress->value();
    dd.PointType = ui->comboBoxPointType->currentPointType();
    dd.Length = ui->lineEditLength->value();
    dd.ScanRate = ui->lineEditScanRate->value();
    ((FormModSca*)parentWidget())->setDisplayDefinition(dd);

    QDialog::accept();
}
