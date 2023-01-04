#include "modbuslimits.h"
#include "dialogsetuppresetdata.h"
#include "ui_dialogsetuppresetdata.h"

///
/// \brief DialogSetupPresetData::DialogSetupPresetData
/// \param params
/// \param parent
///
DialogSetupPresetData::DialogSetupPresetData(DisplayDefinition& params, QWidget *parent) :
     QFixedSizeDialog(parent)
    , ui(new Ui::DialogSetupPresetData)
    ,_params(params)
{
    ui->setupUi(this);
    ui->lineEditSlaveDevice->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditNumberOfPoints->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditSlaveDevice->setValue(params.DeviceId);
    ui->lineEditAddress->setValue(params.PointAddress);
    ui->lineEditNumberOfPoints->setValue(params.Length);

    switch(params.PointType)
    {
        case QModbusDataUnit::Coils:
            setWindowTitle("15: FORCE MULTIPLE COILS");
        break;
        case QModbusDataUnit::HoldingRegisters:
            setWindowTitle("16: FORCE MULTIPLE REGISTERS");
        break;
        default:
        break;
    }

    ui->buttonBox->setFocus();
}

///
/// \brief DialogSetupPresetData::~DialogSetupPresetData
///
DialogSetupPresetData::~DialogSetupPresetData()
{
    delete ui;
}


///
/// \brief DialogSetupPresetData::accept
///
void DialogSetupPresetData::accept()
{
    _params.DeviceId = ui->lineEditSlaveDevice->value<int>();
    _params.PointAddress = ui->lineEditAddress->value<int>();
    _params.Length = ui->lineEditNumberOfPoints->value<int>();
    QFixedSizeDialog::accept();
}
