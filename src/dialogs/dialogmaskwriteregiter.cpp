#include "modbuslimits.h"
#include "dialogmaskwriteregiter.h"
#include "ui_dialogmaskwriteregiter.h"

namespace {
    Qt::CheckState toCheckState(quint16 val) {
        if (val == 0xFFFF) return Qt::Checked;
        if (val == 0x0000) return Qt::Unchecked;
        return Qt::PartiallyChecked;
    }
}

///
/// \brief DialogMaskWriteRegiter::DialogMaskWriteRegiter
/// \param parent
///
DialogMaskWriteRegiter::DialogMaskWriteRegiter(ModbusMaskWriteParams& params, bool hexAddress, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogMaskWriteRegiter)
    ,_writeParams(params)
{
    ui->setupUi(this);

    ui->lineEditNode->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditNode->setValue(params.DeviceId);

    ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditAddress->setValue(params.Address);

    ui->controlAndMask->setValue(params.AndMask);
    ui->controlOrMask->setValue(params.OrMask);

    ui->groupBoxAndMask->setCheckState(toCheckState(params.AndMask));
    ui->groupBoxOrMask->setCheckState(toCheckState(params.OrMask));

    connect(ui->groupBoxAndMask, &CheckableGroupBox::checkStateChanged, this, [this](Qt::CheckState state) {
        ui->controlAndMask->setValue(state == Qt::Checked ? 0xFFFF : 0x0000);
    });
    connect(ui->groupBoxOrMask, &CheckableGroupBox::checkStateChanged, this, [this](Qt::CheckState state) {
        ui->controlOrMask->setValue(state == Qt::Checked ? 0xFFFF : 0x0000);
    });

    connect(ui->controlAndMask, &BitPatternControl::valueChanged, this, [this](quint16 val) {
        ui->groupBoxAndMask->setCheckState(toCheckState(val));
    });
    connect(ui->controlOrMask, &BitPatternControl::valueChanged, this, [this](quint16 val) {
        ui->groupBoxOrMask->setCheckState(toCheckState(val));
    });

    ui->buttonBox->setFocus();
}

///
/// \brief DialogMaskWriteRegiter::~DialogMaskWriteRegiter
///
DialogMaskWriteRegiter::~DialogMaskWriteRegiter()
{
    delete ui;
}

///
/// \brief DialogMaskWriteRegiter::accept
///
void DialogMaskWriteRegiter::accept()
{
    _writeParams.AndMask = ui->controlAndMask->value();
    _writeParams.OrMask = ui->controlOrMask->value();
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.DeviceId = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}
