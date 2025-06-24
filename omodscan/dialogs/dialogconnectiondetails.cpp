#include <QHostInfo>
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
    QFixedSizeDialog(parent)
    , ui(new Ui::DialogConnectionDetails)
    ,_connectionDetails(cd)
{
    ui->setupUi(this);
    ui->lineEditServicePort->setInputRange(0, USHRT_MAX);
    ui->comboBoxConnectUsing->setCurrentIndex(-1);
    ui->comboBoxFlowControl->setCurrentIndex(-1);
    ui->lineEditIPAddress->setText(cd.TcpParams.IPAddress);
    ui->lineEditServicePort->setValue(cd.TcpParams.ServicePort);
    ui->comboBoxBaudRate->setCurrentValue(cd.SerialParams.BaudRate);
    ui->comboBoxWordLength->setCurrentValue(cd.SerialParams.WordLength);
    ui->comboBoxParity->setCurrentParity(cd.SerialParams.Parity);
    ui->comboBoxStopBits->setCurrentValue(cd.SerialParams.StopBits);
    ui->comboBoxFlowControl->setCurrentFlowControl(cd.SerialParams.FlowControl);
    ui->comboBoxDTRControl->setCurrentValue(cd.SerialParams.SetDTR);
    ui->comboBoxRTSControl->setCurrentValue(cd.SerialParams.SetRTS);

    if(cd.Type == ConnectionType::Tcp)
    {
        ui->comboBoxConnectUsing->setCurrentConnectionType(cd.Type, QString());
    }
    else
    {
        ui->comboBoxConnectUsing->setCurrentConnectionType(cd.Type, cd.SerialParams.PortName);
    }

#ifdef Q_OS_WIN
    ui->toolButtonExcludeVirtualPorts->setHidden(true);
#else
    ui->toolButtonExcludeVirtualPorts->setChecked(cd.ExcludeVirtualPorts);
#endif

    ui->buttonBox->setFocus();
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
    _connectionDetails.ExcludeVirtualPorts = ui->toolButtonExcludeVirtualPorts->isChecked();
    if(_connectionDetails.Type == ConnectionType::Tcp)
    {
        if(!QHostAddress(ui->lineEditIPAddress->text()).isNull())
        {
            _connectionDetails.TcpParams.IPAddress = ui->lineEditIPAddress->text();
            _connectionDetails.TcpParams.ServicePort = ui->lineEditServicePort->value<int>();
            QFixedSizeDialog::accept();
        }
        else
        {
            QHostInfo::lookupHost(ui->lineEditIPAddress->text(), this, [this](const QHostInfo &host)
            {
                if (host.error() != QHostInfo::NoError)
                {
                    QMessageBox::warning(this, parentWidget()->windowTitle(), tr("Lookup host failed: ") + host.errorString());
                    return;
                }

                _connectionDetails.TcpParams.IPAddress = ui->lineEditIPAddress->text();
                _connectionDetails.TcpParams.ServicePort = ui->lineEditServicePort->value<int>();
                QFixedSizeDialog::accept();
            });
        }
    }
    else
    {
        _connectionDetails.SerialParams.PortName = ui->comboBoxConnectUsing->currentPortName();
        _connectionDetails.SerialParams.BaudRate = qMax(QSerialPort::Baud1200, (QSerialPort::BaudRate)ui->comboBoxBaudRate->currentValue());
        _connectionDetails.SerialParams.WordLength = (QSerialPort::DataBits)ui->comboBoxWordLength->currentValue();
        _connectionDetails.SerialParams.Parity = ui->comboBoxParity->currentParity();
        _connectionDetails.SerialParams.StopBits = (QSerialPort::StopBits)ui->comboBoxStopBits->currentValue();
        _connectionDetails.SerialParams.FlowControl = ui->comboBoxFlowControl->currentFlowControl();
        _connectionDetails.SerialParams.SetDTR = ui->comboBoxDTRControl->currentValue();
        _connectionDetails.SerialParams.SetRTS = ui->comboBoxRTSControl->currentValue();
        QFixedSizeDialog::accept();
    }
}

///
/// \brief DialogConnectionDetails::on_toolButtonExcludeVirtualPorts_toggled
/// \param checked
///
void DialogConnectionDetails::on_toolButtonExcludeVirtualPorts_toggled(bool checked)
{
    ui->toolButtonExcludeVirtualPorts->setChecked(checked);
    ui->comboBoxConnectUsing->setExcludeVirtuals(checked);
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
    const auto fc = ui->comboBoxFlowControl->currentFlowControl();
    ui->lineEditIPAddress->setEnabled(ct == ConnectionType::Tcp);
    ui->lineEditServicePort->setEnabled(ct == ConnectionType::Tcp);
    ui->comboBoxBaudRate->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxParity->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxStopBits->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxWordLength->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxFlowControl->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxDTRControl->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxRTSControl->setEnabled(ct == ConnectionType::Serial && fc != QSerialPort::HardwareControl);
}

///
/// \brief DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged
///
void DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged(int)
{
    const auto ct = ui->comboBoxConnectUsing->currentConnectionType();
    const auto fc = ui->comboBoxFlowControl->currentFlowControl();
    ui->comboBoxDTRControl->setEnabled(ct == ConnectionType::Serial);
    ui->comboBoxRTSControl->setEnabled(ct == ConnectionType::Serial && fc != QSerialPort::HardwareControl);
}
