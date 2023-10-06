#include <QPushButton>
#include "modbusmessage.h"
#include "dialogmsgparser.h"
#include "ui_dialogmsgparser.h"

///
/// \brief DialogMsgParser::DialogMsgParser
/// \param parent
///
DialogMsgParser::DialogMsgParser(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogMsgParser)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->pduInfo->setShowTimestamp(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Parse"));
}

///
/// \brief DialogMsgParser::~DialogMsgParser
///
DialogMsgParser::~DialogMsgParser()
{
    delete ui;
}

///
/// \brief DialogMsgParser::accept
///
void DialogMsgParser::accept()
{
    if(ui->pduInfo->modbusMessage())
        delete ui->pduInfo->modbusMessage();

    const auto msg = ModbusMessage::parse(ui->pduData->value(), ui->deviceIdIncluded->isChecked(), ui->pduRequest->isChecked());
    ui->pduInfo->setModbusMessage(msg);
}
