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

    ui->comboBoxParity->setItemData(0, QSerialPort::Parity::OddParity);
    ui->comboBoxParity->setItemData(1, QSerialPort::Parity::EvenParity);
    ui->comboBoxParity->setItemData(2, QSerialPort::Parity::NoParity);

    ui->lineEditServicePort->setInputRange(0, 65535);
    ui->lineEditDelatDSR->setInputRange(0, 10);
    ui->lineEditDelayCTS->setInputRange(0, 10);

    ui->lineEditIPAddress->setText(cd.TcpDetails.IPAddress.toString());
    ui->lineEditServicePort->setValue(cd.TcpDetails.ServicePort);

    if(cd.Type == ConnectionType::Tcp)
    {
        on_comboBoxConnectUsing_currentIndexChanged(0);
    }
    else
    {
        ui->comboBoxConnectUsing->setCurrentConnectionType(cd.Type, cd.SerialDetails.PortName);
        ui->comboBoxBaudRate->setCurrentValue(cd.SerialDetails.BaudRate);
        ui->comboBoxWordLength->setCurrentValue(cd.SerialDetails.WordLength);
        ui->comboBoxParity->setCurrentIndex(ui->comboBoxParity->findData(cd.SerialDetails.Parity));
        ui->comboBoxStopBits->setCurrentValue(cd.SerialDetails.StopBits);
        ui->checkBoxDSR->setChecked(cd.SerialDetails.WaitDSR);
        ui->lineEditDelatDSR->setValue(cd.SerialDetails.DelayDSR);
        ui->checkBoxCTS->setChecked(cd.SerialDetails.WaitCTS);
        ui->lineEditDelayCTS->setValue(cd.SerialDetails.DelayCTS);
    }
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
        _connectionDetails.SerialDetails.PortName = ui->comboBoxConnectUsing->currentPortName();
        _connectionDetails.SerialDetails.BaudRate = (QSerialPort::BaudRate)ui->comboBoxBaudRate->currentValue();
        _connectionDetails.SerialDetails.WordLength = (QSerialPort::DataBits)ui->comboBoxWordLength->currentValue();
        _connectionDetails.SerialDetails.Parity = ui->comboBoxParity->currentData().value<QSerialPort::Parity>();
        _connectionDetails.SerialDetails.StopBits = (QSerialPort::StopBits)ui->comboBoxStopBits->currentValue();
        _connectionDetails.SerialDetails.DelayDSR = ui->lineEditDelatDSR->value();
        _connectionDetails.SerialDetails.WaitDSR = ui->checkBoxDSR->isChecked();
        _connectionDetails.SerialDetails.WaitCTS = ui->checkBoxCTS->isChecked();
        _connectionDetails.SerialDetails.DelayCTS = ui->lineEditDelayCTS->value();
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
