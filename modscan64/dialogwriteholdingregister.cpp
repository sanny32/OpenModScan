#include <float.h>
#include <QMessageBox>
#include "modbuslimits.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param params
/// \param mode
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(const ModbusWriteParams& params, DataDisplayMode mode, QWidget* parent) :
      QDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
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

    switch(mode)
    {
        case DataDisplayMode::Binary:
        break;

        case DataDisplayMode::Decimal:
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::Integer:
            ui->lineEditValue->setValue(params.Value.toInt());
        break;

        case DataDisplayMode::Hex:
            ui->labelValue->setText("Value, (HEX): ");
            ui->lineEditValue->setPaddingZeroes(true);
            ui->lineEditValue->setInputMode(NumericLineEdit::HexMode);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditValue->setInputRange(FLT_MIN, FLT_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::RealMode);
            ui->lineEditValue->setValue(params.Value.toFloat());
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditValue->setInputRange(DBL_MIN, DBL_MAX);
            ui->lineEditValue->setInputMode(NumericLineEdit::RealMode);
            ui->lineEditValue->setValue(params.Value.toDouble());
        break;
    }
}

///
/// \brief DialogWriteHoldingRegister::~DialogWriteHoldingRegister
///
DialogWriteHoldingRegister::~DialogWriteHoldingRegister()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegister::showEvent
/// \param e
///
void DialogWriteHoldingRegister::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);
    setFixedSize(sizeHint());
}

///
/// \brief DialogWriteHoldingRegister::writeParams
/// \return
///
ModbusWriteParams DialogWriteHoldingRegister::writeParams() const
{
    const quint32 node = ui->lineEditNode->value<int>();
    const quint32 addr = ui->lineEditAddress->value<int>();
    const auto value = ui->lineEditValue->value<QVariant>();
    return { node, addr, value };
}
