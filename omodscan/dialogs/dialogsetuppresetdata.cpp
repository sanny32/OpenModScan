#include "modbuslimits.h"
#include "dialogsetuppresetdata.h"
#include "ui_dialogsetuppresetdata.h"

///
/// \brief DialogSetupPresetData::DialogSetupPresetData
/// \param params
/// \param pointType
/// \param parent
///
DialogSetupPresetData::DialogSetupPresetData(SetupPresetParams& params,  QModbusDataUnit::RegisterType pointType, QWidget *parent) :
     QFixedSizeDialog(parent)
    , ui(new Ui::DialogSetupPresetData)
    ,_params(params)
{
    ui->setupUi(this);
    ui->lineEditSlaveDevice->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditSlaveDevice->setValue(params.SlaveAddress);
    ui->lineEditAddress->setValue(params.PointAddress);
    ui->lineEditNumberOfPoints->setValue(params.Length);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
            setWindowTitle("15: FORCE MULTIPLE COILS");
            ui->lineEditNumberOfPoints->setInputRange(1, 1968);
        break;
        case QModbusDataUnit::HoldingRegisters:
            setWindowTitle("16: FORCE MULTIPLE REGISTERS");
            ui->lineEditNumberOfPoints->setInputRange(1, 123);
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
    _params.SlaveAddress = ui->lineEditSlaveDevice->value<int>();
    _params.PointAddress = ui->lineEditAddress->value<int>();
    _params.Length = ui->lineEditNumberOfPoints->value<int>();
    QFixedSizeDialog::accept();
}
