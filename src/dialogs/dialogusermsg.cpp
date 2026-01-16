#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>
#include "modbuslimits.h"
#include "dialogusermsg.h"
#include "ui_dialogusermsg.h"

///
/// \brief DialogUserMsg::DialogUserMsg
/// \param dd
/// \param func
/// \param mode
/// \param client
/// \param parent
///
DialogUserMsg::DialogUserMsg(const DisplayDefinition& dd, QModbusPdu::FunctionCode func, DataDisplayMode mode, ModbusClient& client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogUserMsg)
    ,_modbusClient(client)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->lineEditSlaveAddress->setLeadingZeroes(dd.LeadingZeros);
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);

    ui->comboBoxFunction->addItems(ModbusFunction::validCodes());
    ui->comboBoxFunction->setCurrentFunctionCode(func);

    ui->requestInfo->setShowLeadingZeros(dd.LeadingZeros);
    ui->requestInfo->setShowTimestamp(false);
    ui->responseInfo->setShowLeadingZeros(dd.LeadingZeros);
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
    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &DialogUserMsg::on_modbusRequest);
    connect(&_modbusClient, &ModbusClient::modbusResponse, this, &DialogUserMsg::on_modbusResponse);
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
    switch(fc)
    {
        case QModbusPdu::Invalid:
        case QModbusPdu::ReadExceptionStatus:
        case QModbusPdu::GetCommEventCounter:
        case QModbusPdu::GetCommEventLog:
        case QModbusPdu::ReportServerId:
        case QModbusPdu::UndefinedFunctionCode:
            ui->pushButtonGenerate->setEnabled(false);
            ui->sendData->setEnabled(false);
        break;
        default:
            ui->pushButtonGenerate->setEnabled(true);
            ui->sendData->setEnabled(true);
        break;
    }

    ui->sendData->setValue(QByteArray());
    updateRequestInfo();
}

///
/// \brief DialogUserMsg::on_pushButtonGenerate_clicked
///
void DialogUserMsg::on_pushButtonGenerate_clicked()
{

    QByteArray data;
    switch(ui->comboBoxFunction->currentFunctionCode())
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

        case QModbusPdu::ReadFileRecord:
            data = QByteArray("\x07\x06\x00\x01\x00\x00\x00\x02", 8);
            break;

        case QModbusPdu::WriteFileRecord:
            data = QByteArray("\x0B\x06\x00\x01\x00\x00\x00\x02\x00\x0A\x00\x0B", 12);
            break;

        case QModbusPdu::MaskWriteRegister:
            data = QByteArray("\x00\x02\xFF\xFF\x00\x00", 6);
            break;

        case QModbusPdu::ReadWriteMultipleRegisters:
            data = QByteArray("\x00\x64\x00\x02\x00\xC8\x00\x02\x04\x00\x0A\x00\x0B", 13);
            break;

        case QModbusPdu::ReadFifoQueue:
            data = QByteArray("\x00\x0A", 2);
            break;

        case QModbusPdu::EncapsulatedInterfaceTransport:
            data = QByteArray("\x0E\x01\x00", 3);
            break;

        default:
        break;
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

    if(_modbusClient.state() != ModbusDevice::ConnectedState)
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
void DialogUserMsg::on_modbusReply(const ModbusReply* const reply)
{
    if(!reply) return;

    if(0 != reply->requestGroupId())
    {
        return;
    }

    switch(reply->error())
    {
        case ModbusDevice::NoError:
        case ModbusDevice::ProtocolError:
        case ModbusDevice::InvalidResponseError:
        break;

        default:
            QMessageBox::warning(this, windowTitle(), reply->errorString());
        break;
    }

    ui->pushButtonSend->setEnabled(true);
}

///
/// \brief DialogUserMsg::on_modbusRequest
/// \param requestGroupId
/// \param msg
///
void DialogUserMsg::on_modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(requestGroupId != 0)
        return;

    ui->requestInfo->setModbusMessage(msg);
}

///
/// \brief DialogUserMsg::on_modbusResponse
/// \param requestGroupId
/// \param msg
///
void DialogUserMsg::on_modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(requestGroupId != 0)
        return;

    _mm = msg;
    ui->responseBuffer->setValue(*_mm.get());
    ui->responseInfo->setModbusMessage(_mm);
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

    const auto protocol = _modbusClient.protocolType();
    auto msg = ModbusMessage::create(request, protocol, ui->lineEditSlaveAddress->value<int>(), 0, QDateTime::currentDateTime(), true);
    ui->requestInfo->setModbusMessage(msg);
}
