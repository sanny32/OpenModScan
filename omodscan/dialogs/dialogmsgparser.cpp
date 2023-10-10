#include <QPushButton>
#include <QAbstractEventDispatcher>
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
    ,_mm(nullptr)
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
    if(_mm) delete _mm;
}

///
/// \brief DialogMsgParser::on_awake
///
void DialogMsgParser::on_awake()
{
    //ui->rtu->setVisible(ui->buttonAdu->isChecked());
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
    if(_mm) delete _mm;

    auto data = ui->bytesData->value();
    ModbusMessage::Type type = ModbusMessage::Adu;
    const auto protocol = ui->checksum->isChecked() ? QModbusAdu::Rtu : QModbusAdu::Tcp;

    if(ui->buttonPdu->isChecked())
    {
        type = ModbusMessage::Pdu;
        if(!ui->deviceIdIncluded->isChecked()) data.push_front('\0');
    }

    _mm = ModbusMessage::parse(data, type, protocol, ui->request->isChecked());
    ui->info->setModbusMessage(_mm);
}
