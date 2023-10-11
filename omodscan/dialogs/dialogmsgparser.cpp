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
/// \brief DialogMsgParser::on_bytesData_valueChanged
/// \param value
///
void DialogMsgParser::on_bytesData_valueChanged(const QByteArray& value)
{
    if(value.size() < 3)
    {
        if(value.isEmpty())
            ui->info->clear();

        return;
    }

    const auto data = value.left(value.size() - 2);
    const int checksum = makeWord(value[value.size() - 1], value[value.size() - 2], ByteOrder::LittleEndian);
    ui->checksumIncluded->setChecked(QModbusAdu::calculateCRC(data, data.size()) == checksum);
}

///
/// \brief DialogMsgParser::accept
///
void DialogMsgParser::accept()
{
    auto data = ui->bytesData->value();
    if(data.isEmpty()) return;

    ModbusMessage::Type type = ModbusMessage::Adu;
    auto protocol = QModbusAdu::Tcp;
    int checksum = 0;

    if(ui->buttonPdu->isChecked())
    {
        type = ModbusMessage::Pdu;
    }

    if(!ui->deviceIdIncluded->isChecked())
    {
        data.push_front('\0');
    }

    if(ui->checksumIncluded->isChecked())
    {
        protocol = QModbusAdu::Rtu;
        checksum = makeWord(data[data.size() - 1], data[data.size() - 2], ByteOrder::LittleEndian);
        data = data.left(data.size() - 2);
    }

    if(_mm) delete _mm;
    _mm = ModbusMessage::parse(data, type, protocol, ui->request->isChecked(), checksum);
    ui->info->setModbusMessage(_mm);
}
