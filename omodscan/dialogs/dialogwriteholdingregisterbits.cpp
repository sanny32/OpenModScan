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
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);

    setValue(params.Value.toUInt());
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
    quint16 value = 0;
    for (int i = 0; i < 16; i++)
    {
        auto ctrl = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(ctrl) value |= ctrl->isChecked() << i;
    }
    _writeParams.Value = value;
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Node = ui->lineEditNode->value<int>();

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
    if(cli != nullptr && cli->state() == QModbusDevice::ConnectedState) {
        setValue(cli->readRegister(QModbusDataUnit::HoldingRegisters, address, ui->lineEditNode->value<int>()));
    }
}

///
/// \brief DialogWriteHoldingRegisterBits::setValue
/// \param value
///
void DialogWriteHoldingRegisterBits::setValue(const quint16 value)
{
    for (int i = 0; i < 16; i++)
    {
        auto ctrl = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(ctrl) ctrl->setChecked(value >> i & 1);
    }
}
