#include <QtDebug>
#include "formmodsca.h"
#include "ui_formmodsca.h"

///
/// \brief FormModSca::FormModSca
/// \param parent
///
FormModSca::FormModSca(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::FormModSca)
{
    ui->setupUi(this);
    ui->labelStatus->setText("** Data Uninitialized **");
    ui->lineEditAddress->setInputRange(1, 65534);
    ui->lineEditAddress->allowPaddingZeroFormatter(true);
    ui->lineEditLegth->setInputRange(1, 128);
    ui->lineEditDeviceId->setInputRange(1, 255);

    updateListWidget();
}

///
/// \brief FormModSca::~FormModSca
///
FormModSca::~FormModSca()
{
    delete ui;
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_currentTextChanged
///
void FormModSca::on_comboBoxModbusPointType_currentTextChanged(const QString&)
{
    updateListWidget();
}

///
/// \brief FormModSca::updateListWidget
///
void FormModSca::updateListWidget()
{
    QString prefix;
    auto modbusPointType = ui->comboBoxModbusPointType->currentText().split(':')[0].toInt();
    switch(modbusPointType)
    {
        case 2:
            prefix = "1";
        break;
        case 3:
            prefix = "4";
        break;
        case 4:
            prefix = "3";
        break;
    }

    ui->listWidget->clear();
    for(int i = 0; i < ui->lineEditLegth->value(); i++)
    {
        const auto addr = QStringLiteral("%1").arg(i + 1, 4, 10, QLatin1Char('0'));
        const auto value = QStringLiteral("%1").arg(0, 5, 10, QLatin1Char('0'));
        const auto label = QString("%1%2: <%3>                ").arg(prefix, addr, value);
        ui->listWidget->addItem(label);
    }
}
