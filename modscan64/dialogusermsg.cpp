#include <QPushButton>
#include <QDialogButtonBox>
#include "modbuslimits.h"
#include "dialogusermsg.h"
#include "ui_dialogusermsg.h"

///
/// \brief DialogUserMsg::DialogUserMsg
/// \param slaveAddress
/// \param mode
/// \param parent
///
DialogUserMsg::DialogUserMsg(quint8 slaveAddress, DataDisplayMode mode, QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogUserMsg)
{
    ui->setupUi(this);

    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditSlaveAddress->setValue(slaveAddress);
    ui->lineEditFunction->setInputRange(0, 255);

    switch(mode)
    {
        case DataDisplayMode::Binary:
        case DataDisplayMode::Hex:
            ui->radioButtonHex->click();
        break;
        default:
            ui->radioButtonDecimal->click();
        break;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Send");
}

///
/// \brief DialogUserMsg::~DialogUserMsg
///
DialogUserMsg::~DialogUserMsg()
{
    delete ui;
}

///
/// \brief DialogUserMsg::accept
///
void DialogUserMsg::accept()
{
}

///
/// \brief DialogUserMsg::on_radioButtonHex_clicked
/// \param checked
///
void DialogUserMsg::on_radioButtonHex_clicked(bool checked)
{
    if(checked)
    {
        //ui->lineEditSlaveAddress->setPaddingZeroes(true);
        //ui->lineEditFunction->setPaddingZeroes(true);
        ui->lineEditSlaveAddress->setInputMode(NumericLineEdit::HexMode);
        ui->lineEditFunction->setInputMode(NumericLineEdit::HexMode);
    }
}

///
/// \brief DialogUserMsg::on_radioButtonDecimal_clicked
/// \param checked
///
void DialogUserMsg::on_radioButtonDecimal_clicked(bool checked)
{
    if(checked)
    {
        ui->lineEditSlaveAddress->setPaddingZeroes(false);
        ui->lineEditFunction->setPaddingZeroes(false);
        ui->lineEditSlaveAddress->setInputMode(NumericLineEdit::IntMode);
        ui->lineEditFunction->setInputMode(NumericLineEdit::IntMode);
    }
}
