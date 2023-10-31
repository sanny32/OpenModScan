#include <QPushButton>
#include <QAbstractEventDispatcher>
#include "dialogmsgparser.h"
#include "ui_dialogmsgparser.h"

///
/// \brief DialogMsgParser::DialogMsgParser
/// \param mode
/// \param protocol
/// \param parent
///
DialogMsgParser::DialogMsgParser(DataDisplayMode mode, ModbusMessage::ProtocolType protocol, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogMsgParser)
    ,_mm(nullptr)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->info->setShowTimestamp(false);
    ui->hexView->setCheckState(mode == DataDisplayMode::Hex ? Qt::Checked : Qt::Unchecked);
    ui->buttonRtu->setChecked(protocol == ModbusMessage::Rtu);
    ui->buttonTcp->setChecked(protocol == ModbusMessage::Tcp);

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
/// \brief DialogMsgParser::changeEvent
/// \param event
///
void DialogMsgParser::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QDialog::changeEvent(event);
}

///
/// \brief DialogMsgParser::on_awake
///
void DialogMsgParser::on_awake()
{
    ui->pushButtonParse->setEnabled(!ui->bytesData->isEmpty());
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
}

///
/// \brief DialogMsgParser::on_pushButtonParse_clicked
///
void DialogMsgParser::on_pushButtonParse_clicked()
{
    auto data = ui->bytesData->value();
    if(data.isEmpty()) return;

    auto protocol = ModbusMessage::Rtu;
    if(ui->buttonTcp->isChecked())
        protocol = ModbusMessage::Tcp;

    if(_mm) delete _mm;
    _mm = ModbusMessage::create(data, protocol, QDateTime::currentDateTime(), ui->request->isChecked());
    ui->info->setModbusMessage(_mm);
}
