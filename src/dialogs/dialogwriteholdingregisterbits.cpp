#include <QBitArray>
#include "modbuslimits.h"
#include "modbusclient.h"
#include "waitcursor.h"
#include "checkablegroupbox.h"
#include "dialogwriteholdingregisterbits.h"
#include "ui_dialogwriteholdingregisterbits.h"

namespace {
    Qt::CheckState toCheckState(quint16 val) {
        if(val == 0xFFFF) return Qt::Checked;
        if(val == 0x0000) return Qt::Unchecked;
        return Qt::PartiallyChecked;
    }
}

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

    ui->groupBox->setCheckState(toCheckState(params.Value.toUInt()));
    connect(ui->groupBox, &CheckableGroupBox::checkStateChanged, this, [this](Qt::CheckState state) {
        ui->controlBitPattern->setValue(state == Qt::Checked ? 0xFFFF : 0x0000);
    });
    connect(ui->controlBitPattern, &BitPatternControl::valueChanged, this, [this](quint16 val) {
        ui->groupBox->setCheckState(toCheckState(val));
    });

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
/// \brief DialogWriteHoldingRegisterBits::readValue
/// \param address
/// \param deviceId
///
void DialogWriteHoldingRegisterBits::readValue(int address, int deviceId)
{
    ModbusClient* cli = _writeParams.Client;
    if(cli == nullptr || cli->state() != ModbusDevice::ConnectedState) return;

    WaitCursor wait(this);
    ui->controlBitPattern->setValue(cli->syncReadRegister(QModbusDataUnit::HoldingRegisters, address, deviceId));
}

///
/// \brief DialogWriteHoldingRegisterBits::on_lineEditAddress_valueChanged
/// \param value
///
void DialogWriteHoldingRegisterBits::on_lineEditAddress_valueChanged(const QVariant& value)
{
    const int address = value.toInt() - (_writeParams.ZeroBasedAddress ? 0 : 1);
    readValue(address, ui->lineEditNode->value<int>());
}

///
/// \brief DialogWriteHoldingRegisterBits::on_lineEditNode_valueChanged
/// \param value
///
void DialogWriteHoldingRegisterBits::on_lineEditNode_valueChanged(const QVariant& value)
{
    const int address = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
    readValue(address, value.toInt());
}
