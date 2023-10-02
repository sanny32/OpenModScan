#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>
#include "modbuslimits.h"
#include "dialogusermsg.h"
#include "ui_dialogusermsg.h"

///
/// \brief DialogUserMsg::DialogUserMsg
/// \param slaveAddress
/// \param func
/// \param mode
/// \param client
/// \param parent
///
DialogUserMsg::DialogUserMsg(quint8 slaveAddress, QModbusPdu::FunctionCode func, DataDisplayMode mode, ModbusClient& client, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogUserMsg)
    ,_modbusClient(client)
{
    ui->setupUi(this);

    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditSlaveAddress->setValue(slaveAddress);
    ui->lineEditFunction->setInputRange(0, 255);
    ui->lineEditFunction->setValue(func);

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

    connect(&_modbusClient, &ModbusClient::modbusReply, this, &DialogUserMsg::on_modbusReply);
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
    ui->lineEditResponse->clear();

    if(_modbusClient.state() != QModbusDevice::ConnectedState)
    {
        QMessageBox::warning(this, parentWidget()->windowTitle(), "Custom Cmd Write Failure");
        return;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    QModbusRequest request;
    request.setFunctionCode(ui->lineEditFunction->value<QModbusPdu::FunctionCode>());
    request.setData(ui->lineEditSendData->value());

    _modbusClient.sendRawRequest(request, ui->lineEditSlaveAddress->value<int>(), 0);

    const auto timeout = _modbusClient.timeout() * _modbusClient.numberOfRetries();
    QTimer::singleShot(timeout, this, [&] { ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true); });
}

///
/// \brief DialogUserMsg::on_modbusReply
/// \param reply
///
void DialogUserMsg::on_modbusReply(QModbusReply* reply)
{
    if(!reply) return;

    if(0 != reply->property("RequestId").toInt())
    {
        return;
    }

    const auto raw = reply->rawResult();

    QByteArray data;
    data.push_back(reply->serverAddress());
    data.push_back(raw.functionCode() | ( raw.isException() ? QModbusPdu::ExceptionByte : 0));
    data.push_back(raw.data());

    ui->lineEditResponse->setValue(data);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

///
/// \brief DialogUserMsg::on_radioButtonHex_clicked
/// \param checked
///
void DialogUserMsg::on_radioButtonHex_clicked(bool checked)
{
    if(checked)
    {
        ui->lineEditSlaveAddress->setPaddingZeroes(true);
        ui->lineEditSlaveAddress->setInputMode(NumericLineEdit::HexMode);
        ui->lineEditFunction->setPaddingZeroes(true);
        ui->lineEditFunction->setInputMode(NumericLineEdit::HexMode);
        ui->lineEditSendData->setInputMode(ByteListLineEdit::HexMode);
        ui->lineEditResponse->setInputMode(ByteListLineEdit::HexMode);
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
        ui->lineEditSlaveAddress->setInputMode(NumericLineEdit::DecMode);
        ui->lineEditFunction->setPaddingZeroes(false);
        ui->lineEditFunction->setInputMode(NumericLineEdit::DecMode);
        ui->lineEditSendData->setInputMode(ByteListLineEdit::DecMode);
        ui->lineEditResponse->setInputMode(ByteListLineEdit::DecMode);
    }
}
