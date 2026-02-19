#include <QBitArray>
#include "modbuslimits.h"
#include "modbusclient.h"
#include "waitcursor.h"
#include "dialogwriteholdingregisterbits.h"
#include "ui_dialogwriteholdingregisterbits.h"

///
/// \brief DialogWriteHoldingRegisterBits::DialogWriteHoldingRegisterBits
/// \param params
/// \param hexAddress
/// \param parent
///
DialogWriteHoldingRegisterBits::DialogWriteHoldingRegisterBits(ModbusWriteParams& params, bool hexAddress, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegisterBits)
    ,_writeParams(params)
{
    ui->setupUi(this);

    if(params.ForceModbus15And16Func) {
        setWindowTitle(tr("16: Write Holding Register Bits"));
    }

    ui->lineEditNode->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditNode->setValue(params.DeviceId);

    ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditAddress->setValue(params.Address);

    ui->controlBitPattern->setValue(params.Value.toUInt());
    ui->buttonBox->setFocus();
}

///
/// \brief DialogWriteHoldingRegisterBits::~DialogWriteHoldingRegisterBits
///
DialogWriteHoldingRegisterBits::~DialogWriteHoldingRegisterBits()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegisterBits::accept
///
void DialogWriteHoldingRegisterBits::accept()
{
    _writeParams.Value = ui->controlBitPattern->value();
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.DeviceId = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}

///
/// \brief DialogWriteHoldingRegisterBits::on_lineEditAddress_valueChanged
/// \param value
///
void DialogWriteHoldingRegisterBits::on_lineEditAddress_valueChanged(const QVariant& value)
{
    WaitCursor wait(this);

    const quint32 address = value.toUInt();
    ModbusClient* cli = _writeParams.Client;
    if(cli != nullptr && cli->state() == ModbusDevice::ConnectedState) {
        ui->controlBitPattern->setValue(cli->syncReadRegister(QModbusDataUnit::HoldingRegisters, address, ui->lineEditNode->value<int>()));
    }
}
