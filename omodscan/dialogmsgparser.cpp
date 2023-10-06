#include <QPushButton>
#include "dialogmsgparser.h"
#include "ui_dialogmsgparser.h"

///
/// \brief DialogMsgParser::DialogMsgParser
/// \param parent
///
DialogMsgParser::DialogMsgParser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMsgParser)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

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
    const auto data = ui->pduData->value();
}
