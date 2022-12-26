#include "dialogconnectiondetails.h"
#include "ui_dialogconnectiondetails.h"

///
/// \brief DialogConnectionDetails::DialogConnectionDetails
/// \param parent
///
DialogConnectionDetails::DialogConnectionDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnectionDetails)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());
    on_comboBoxConnectUsing_currentIndexChanged(0);
}

///
/// \brief DialogConnectionDetails::~DialogConnectionDetails
///
DialogConnectionDetails::~DialogConnectionDetails()
{
    delete ui;
}

///
/// \brief DialogConnectionDetails::on_comboBoxConnectUsing_currentIndexChanged
///
void DialogConnectionDetails::on_comboBoxConnectUsing_currentIndexChanged(int)
{
    const auto ct = ui->comboBoxConnectUsing->currentConnectionType();
    ui->lineEditIPAddress->setEnabled(ct == ConnectionType::Tcp);
    ui->lineEditServicePort->setEnabled(ct == ConnectionType::Tcp);
    ui->comboBoxBaudRate->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxParity->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxStopBits->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxWordLength->setEnabled(ct == ConnectionType::Serial);
    ui->checkBoxDSR->setEnabled(ct == ConnectionType::Serial);
    ui->checkBoxCTS->setEnabled(ct == ConnectionType::Serial);
    ui->lineEditDelatDSR->setEnabled(ct == ConnectionType::Serial);
    ui->lineEditDelayCTS->setEnabled(ct == ConnectionType::Serial);
}
