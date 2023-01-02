#include "modbuslimits.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param node
/// \param addr
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(const WriteRegisterParams& params, QModbusClient* client, QWidget* parent) :
      QDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
    ,_modbusClient(client)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());

    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditValue->setInputRange(0, 65535);
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);
    ui->lineEditValue->setValue(params.Value.toInt());
}

///
/// \brief DialogWriteHoldingRegister::~DialogWriteHoldingRegister
///
DialogWriteHoldingRegister::~DialogWriteHoldingRegister()
{
    delete ui;
}
