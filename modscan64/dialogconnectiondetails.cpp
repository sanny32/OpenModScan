#include "dialogprotocolselections.h"
#include "dialogconnectiondetails.h"
#include "ui_dialogconnectiondetails.h"

///
/// \brief DialogConnectionDetails::DialogConnectionDetails
/// \param _cd
/// \param parent
///
DialogConnectionDetails::DialogConnectionDetails(ConnectionDetails& cd, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::DialogConnectionDetails)
    ,_connectionDetails(cd)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
    setFixedSize(size());
    ui->lineEditServicePort->setInputRange(0, 65535);
    ui->lineEditDelatDSR->setInputRange(0, 10);
    ui->lineEditDelayCTS->setInputRange(0, 10);
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
/// \brief DialogConnectionDetails::accept
///
void DialogConnectionDetails::accept()
{
    _connectionDetails.Type = ui->comboBoxConnectUsing->currentConnectionType();
    if(_connectionDetails.Type == ConnectionType::Tcp)
    {
        _connectionDetails.TcpDetails.IPAddress = QHostAddress(ui->labelIPAddress->text());
        _connectionDetails.TcpDetails.ServicePort = ui->lineEditServicePort->value();
    }
    else
    {

    }

    QDialog::accept();
}

///
/// \brief DialogConnectionDetails::on_pushButtonProtocolSelections_clicked
///
void DialogConnectionDetails::on_pushButtonProtocolSelections_clicked()
{
    DialogProtocolSelections dlg(_connectionDetails.ProtocolSelections, this);
    dlg.exec();
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
