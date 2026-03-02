#include "modbuslimits.h"
#include "dialogsetuppresetdata.h"
#include "ui_dialogsetuppresetdata.h"

///
/// \brief DialogSetupPresetData::DialogSetupPresetData
/// \param params
/// \param pointType
/// \param hexAddress
/// \param parent
///
DialogSetupPresetData::DialogSetupPresetData(SetupPresetParams& params,  QModbusDataUnit::RegisterType pointType, bool hexAddress, QWidget *parent) :
     QFixedSizeDialog(parent)
    , ui(new Ui::DialogSetupPresetData)
    , _params(params)
    , _pointType(pointType)
{
    ui->setupUi(this);

    ui->lineEditSlaveDevice->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditSlaveDevice->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditSlaveDevice->setValue(params.DeviceId);

    ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditAddress->setValue(params.PointAddress);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
            setWindowTitle(tr("15: WRITE MULTIPLE COILS"));
        break;
        case QModbusDataUnit::HoldingRegisters:
            setWindowTitle(tr("16: WRITE MULTIPLE REGISTERS"));
        break;
        default:
        break;
    }

    const int typeMax = (pointType == QModbusDataUnit::Coils) ? 1968 : 123;
    const int offset = params.PointAddress - (params.ZeroBasedAddress ? 0 : 1);
    const int maxLen = qMin(typeMax, 65536 - offset);
    ui->lineEditNumberOfPoints->setInputRange(1, qMax(1, maxLen));
    ui->lineEditNumberOfPoints->setValue(params.Length);

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
/// \brief DialogSetupPresetData::on_lineEditAddress_valueChanged
///
void DialogSetupPresetData::on_lineEditAddress_valueChanged(const QVariant&)
{
    const int address = ui->lineEditAddress->value<int>();
    const int offset = address - (_params.ZeroBasedAddress ? 0 : 1);
    const int typeMax = (_pointType == QModbusDataUnit::Coils) ? 1968 : 123;
    const int maxLen = qMin(typeMax, 65536 - offset);
    const int newMax = qMax(1, maxLen);

    ui->lineEditNumberOfPoints->setInputRange(1, newMax);
    if(ui->lineEditNumberOfPoints->value<int>() > newMax)
        ui->lineEditNumberOfPoints->setValue(newMax);
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
