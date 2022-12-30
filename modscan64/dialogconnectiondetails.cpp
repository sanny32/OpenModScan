#include <QMessageBox>
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

    ui->comboBoxConnectUsing->setCurrentIndex(-1);
    ui->comboBoxParity->setItemData(0, QSerialPort::OddParity);
    ui->comboBoxParity->setItemData(1, QSerialPort::EvenParity);
    ui->comboBoxParity->setItemData(2, QSerialPort::NoParity);

    ui->comboBoxFlowControl->setCurrentIndex(-1);
    ui->comboBoxFlowControl->setItemData(0, QSerialPort::NoFlowControl);
    ui->comboBoxFlowControl->setItemData(1, QSerialPort::HardwareControl);
    ui->comboBoxFlowControl->setItemData(2, QSerialPort::SoftwareControl);

    ui->lineEditIPAddress->setText(cd.TcpParams.IPAddress);
    ui->lineEditServicePort->setValue(cd.TcpParams.ServicePort);

    ui->comboBoxBaudRate->setCurrentValue(cd.SerialParams.BaudRate);
    ui->comboBoxWordLength->setCurrentValue(cd.SerialParams.WordLength);
    ui->comboBoxParity->setCurrentIndex(ui->comboBoxParity->findData(cd.SerialParams.Parity));
    ui->comboBoxStopBits->setCurrentValue(cd.SerialParams.StopBits);
    ui->comboBoxFlowControl->setCurrentIndex(ui->comboBoxFlowControl->findData(cd.SerialParams.FlowControl));
    ui->checkBoxDTR->setChecked(cd.SerialParams.SetDTR);
    ui->checkBoxRTS->setChecked(cd.SerialParams.SetRTS);

    if(cd.Type == ConnectionType::Tcp)
    {
        ui->comboBoxConnectUsing->setCurrentConnectionType(cd.Type, QString());
    }
    else
    {
        ui->comboBoxConnectUsing->setCurrentConnectionType(cd.Type, cd.SerialParams.PortName);
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
        const auto ipAddr = QHostAddress(ui->lineEditIPAddress->text());
        if(ipAddr.isNull())
        {
            QMessageBox::warning(this, parentWidget()->windowTitle(), "Invalid IP Address");
            return;
        }

        _connectionDetails.TcpParams.IPAddress = ipAddr.toString();
        _connectionDetails.TcpParams.ServicePort = ui->lineEditServicePort->value();
    }
    else
    {
        _connectionDetails.SerialParams.PortName = ui->comboBoxConnectUsing->currentPortName();
        _connectionDetails.SerialParams.BaudRate = (QSerialPort::BaudRate)ui->comboBoxBaudRate->currentValue();
        _connectionDetails.SerialParams.WordLength = (QSerialPort::DataBits)ui->comboBoxWordLength->currentValue();
        _connectionDetails.SerialParams.Parity = ui->comboBoxParity->currentData().value<QSerialPort::Parity>();
        _connectionDetails.SerialParams.StopBits = (QSerialPort::StopBits)ui->comboBoxStopBits->currentValue();
        _connectionDetails.SerialParams.FlowControl = ui->comboBoxFlowControl->currentData().value<QSerialPort::FlowControl>();
        _connectionDetails.SerialParams.SetDTR = ui->checkBoxDTR->isChecked();
        _connectionDetails.SerialParams.SetRTS = ui->checkBoxRTS->isChecked();
    }

    QDialog::accept();
}

///
/// \brief DialogConnectionDetails::on_pushButtonProtocolSelections_clicked
///
void DialogConnectionDetails::on_pushButtonProtocolSelections_clicked()
{
    DialogProtocolSelections dlg(_connectionDetails.ModbusParams, this);
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
    ui->comboBoxFlowControl->setEnabled(ct == ConnectionType::Serial);
    ui->checkBoxDTR->setEnabled(ct == ConnectionType::Serial);
    ui->checkBoxRTS->setEnabled(ct == ConnectionType::Serial);
}

///
/// \brief DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged
///
void DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged(int)
{
    /*const auto ct = ui->comboBoxConnectUsing->currentConnectionType();
    const auto flowCtrl = ui->comboBoxFlowControl->currentData().value<QSerialPort::FlowControl>();
    ui->checkBoxDTR->setEnabled(ct == ConnectionType::Serial && flowCtrl == QSerialPort::HardwareControl);
    ui->checkBoxRTS->setEnabled(ct == ConnectionType::Serial && flowCtrl == QSerialPort::HardwareControl);*/
}
