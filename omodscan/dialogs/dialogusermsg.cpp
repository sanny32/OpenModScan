#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>
#include "fontutils.h"
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
DialogUserMsg::DialogUserMsg(quint8 slaveAddress, QModbusPdu::FunctionCode func, DataDisplayMode mode, ModbusClient& client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogUserMsg)
    ,_modbusClient(client)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditSlaveAddress->setValue(slaveAddress);
    ui->comboBoxFunction->addItems(ModbusFunction::validCodes());
    ui->comboBoxFunction->setCurrentFunctionCode(func);
    ui->requestInfo->setShowTimestamp(false);
    ui->responseInfo->setShowTimestamp(false);

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

    ui->sendData->setFocus();
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
/// \brief DialogUserMsg::changeEvent
/// \param event
///
void DialogUserMsg::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QDialog::changeEvent(event);
}

///
/// \brief DialogUserMsg::on_lineEditSlaveAddress_valueChanged
///
void DialogUserMsg::on_lineEditSlaveAddress_valueChanged(const QVariant&)
{
    updateRequestInfo();
}

///
/// \brief DialogUserMsg::on_comboBoxFunction_functionCodeChanged
/// \param fc
///
void DialogUserMsg::on_comboBoxFunction_functionCodeChanged(QModbusPdu::FunctionCode fc)
{
    QByteArray data;
    switch(fc)
    {
        case QModbusPdu::ReadCoils:
        case QModbusPdu::ReadDiscreteInputs:
        case QModbusPdu::ReadInputRegisters:
        case QModbusPdu::ReadHoldingRegisters:
            data = QByteArray("\x00\x01\x00\x02", 4);
        break;

        case QModbusPdu::WriteSingleCoil:
            data = QByteArray("\x00\x02\xFF\x00", 4);
        break;

        case QModbusPdu::WriteSingleRegister:
            data = QByteArray("\x00\x01\x00\x03", 4);
        break;

        case QModbusPdu::Diagnostics:
            data = QByteArray("\x00\x02\x00\x00", 4);
        break;

        case QModbusPdu::WriteMultipleCoils:
            data = QByteArray("\x00\x01\x00\x03\x01\x07", 6);
        break;

        case QModbusPdu::WriteMultipleRegisters:
            data = QByteArray("\x00\x01\x00\x03\x06\xF8\xB7\xEF\x7E\xDB\x72", 11);
        break;

        case QModbusPdu::MaskWriteRegister:
            data = QByteArray("\x00\x02\xFF\xFF\x00\x00", 6);
        break;

        default: break;
    }

    ui->sendData->blockSignals(true);
    ui->sendData->setValue(data);
    ui->sendData->blockSignals(false);

    updateRequestInfo();
}

///
/// \brief DialogUserMsg::on_sendData_valueChanged
///
void DialogUserMsg::on_sendData_valueChanged(const QByteArray&)
{
    updateRequestInfo();
}

///
/// \brief DialogUserMsg::on_pushButtonSend_clicked
///
void DialogUserMsg::on_pushButtonSend_clicked()
{
    ui->responseBuffer->clear();
    ui->responseInfo->clear();

    if(_modbusClient.state() != QModbusDevice::ConnectedState)
    {
        QMessageBox::warning(this, windowTitle(), tr("No connection to device"));
        return;
    }

    ui->pushButtonSend->setEnabled(false);

    auto msg = ui->requestInfo->modbusMessage();
    const QModbusRequest request = msg->adu()->pdu();
    _modbusClient.sendRawRequest(request, ui->lineEditSlaveAddress->value<int>(), 0);

    const auto timeout = _modbusClient.timeout() * _modbusClient.numberOfRetries();
    QTimer::singleShot(timeout, this, [&] { ui->pushButtonSend->setEnabled(true); });
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

    if(reply->error() != QModbusDevice::NoError &&
        reply->error() != QModbusDevice::ProtocolError)
    {
        QMessageBox::warning(this, windowTitle(), reply->errorString());
        return;
    }

    const auto protocol = _modbusClient.connectionType() == ConnectionType::Tcp ? ModbusMessage::Tcp : ModbusMessage::Rtu;
    _mm = ModbusMessage::create(reply->rawResult(), protocol, reply->serverAddress(), QDateTime::currentDateTime(), false);

    if(protocol == ModbusMessage::Tcp)
        ((QModbusAduTcp*)_mm->adu())->setTransactionId(reply->property("TransactionId").toInt());

    ui->responseBuffer->setValue(*_mm.get());
    ui->responseInfo->setModbusMessage(_mm);

    ui->pushButtonSend->setEnabled(true);
}

///
/// \brief DialogUserMsg::on_radioButtonHex_clicked
/// \param checked
///
void DialogUserMsg::on_radioButtonHex_clicked(bool checked)
{
    if(checked)
    {
        ui->comboBoxFunction->setInputMode(FunctionCodeComboBox::HexMode);
        ui->sendData->setInputMode(ByteListTextEdit::HexMode);
        ui->requestInfo->setDataDisplayMode(DataDisplayMode::Hex);
        ui->responseBuffer->setInputMode(ByteListTextEdit::HexMode);
        ui->responseInfo->setDataDisplayMode(DataDisplayMode::Hex);
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
        ui->comboBoxFunction->setInputMode(FunctionCodeComboBox::DecMode);
        ui->sendData->setInputMode(ByteListTextEdit::DecMode);
        ui->requestInfo->setDataDisplayMode(DataDisplayMode::UInt16);
        ui->responseBuffer->setInputMode(ByteListTextEdit::DecMode);
        ui->responseInfo->setDataDisplayMode(DataDisplayMode::UInt16);
    }
}

///
/// \brief DialogUserMsg::updateRequestInfo
///
void DialogUserMsg::updateRequestInfo()
{
    QModbusRequest request;
    request.setFunctionCode(ui->comboBoxFunction->currentFunctionCode());
    request.setData(ui->sendData->value());

    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    auto msg = ModbusMessage::create(request, protocol, ui->lineEditSlaveAddress->value<int>(), QDateTime::currentDateTime(), true);
    ui->requestInfo->setModbusMessage(msg);
}
