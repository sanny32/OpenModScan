#include "dialogmsgparser.h"
#include "ui_dialogmsgparser.h"

DialogMsgParser::DialogMsgParser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMsgParser)
{
    ui->setupUi(this);
}

DialogMsgParser::~DialogMsgParser()
{
    delete ui;
}
