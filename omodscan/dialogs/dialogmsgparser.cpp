#include <QPushButton>
#include <QAbstractEventDispatcher>
#include "modbusmessage.h"
#include "dialogmsgparser.h"
#include "ui_dialogmsgparser.h"

///
/// \brief DialogMsgParser::DialogMsgParser
/// \param mode
/// \param parent
///
DialogMsgParser::DialogMsgParser(DataDisplayMode mode, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogMsgParser)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->info->setShowTimestamp(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Parse"));
    ui->hexView->setCheckState(mode == DataDisplayMode::Hex ? Qt::Checked : Qt::Unchecked);

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogMsgParser::on_awake);
}

///
/// \brief DialogMsgParser::~DialogMsgParser
///
DialogMsgParser::~DialogMsgParser()
{
    delete ui;
}

///
/// \brief DialogMsgParser::on_awake
///
void DialogMsgParser::on_awake()
{
    ui->rtu->setVisible(ui->buttonAdu->isChecked());
    ui->deviceIdIncluded->setVisible(ui->buttonPdu->isChecked());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!ui->bytesData->isEmpty());
}

///
/// \brief DialogMsgParser::on_hexView_toggled
/// \param checked
///
void DialogMsgParser::on_hexView_toggled(bool checked)
{
    ui->bytesData->setInputMode(checked ? ByteListTextEdit::HexMode : ByteListTextEdit::DecMode);
    ui->info->setDataDisplayMode(checked ? DataDisplayMode::Hex : DataDisplayMode::Decimal);
}

///
/// \brief DialogMsgParser::accept
///
void DialogMsgParser::accept()
{
    if(ui->info->modbusMessage())
        delete ui->info->modbusMessage();

    const ModbusMessage* msg = nullptr;
    if(ui->buttonPdu->isChecked())
    {
        auto data = ui->bytesData->value();
        if(!ui->deviceIdIncluded->isChecked()) data.push_front('\0');
        msg = ModbusMessage::parse(data, ModbusMessage::Pdu, QModbusAdu::Tcp, ui->request->isChecked());
    }
    else if(ui->buttonAdu->isChecked())
    {
        auto data = ui->bytesData->value();
        const auto protocol = ui->rtu->isChecked() ? QModbusAdu::Rtu : QModbusAdu::Tcp;
        msg = ModbusMessage::parse(data, ModbusMessage::Adu, protocol, ui->request->isChecked());
    }

    ui->info->setModbusMessage(msg);
}
