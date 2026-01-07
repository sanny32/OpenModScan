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
    ,_connType(cd.Type)
    ,_connectionDetails(cd)
{
    ui->setupUi(this);
    ui->lineEditServicePort->setInputRange(0, USHRT_MAX);
    ui->comboBoxConnectUsing->setCurrentIndex(-1);
    ui->comboBoxFlowControl->setCurrentIndex(-1);
    ui->comboBoxIPAddress->setCurrentText(cd.TcpParams.IPAddress);
    ui->lineEditServicePort->setValue(cd.TcpParams.ServicePort);
    ui->comboBoxSerialPort->setCurrentText(cd.SerialParams.PortName);
    ui->comboBoxBaudRate->setCurrentValue(cd.SerialParams.BaudRate);
    ui->comboBoxWordLength->setCurrentValue(cd.SerialParams.WordLength);
    ui->comboBoxParity->setCurrentParity(cd.SerialParams.Parity);
    ui->comboBoxStopBits->setCurrentValue(cd.SerialParams.StopBits);
    ui->comboBoxFlowControl->setCurrentFlowControl(cd.SerialParams.FlowControl);
    ui->comboBoxDTRControl->setCurrentValue(cd.SerialParams.SetDTR);
    ui->comboBoxRTSControl->setCurrentValue(cd.SerialParams.SetRTS);

    if(cd.Type == ConnectionType::Tcp)
    {
        ui->comboBoxConnectUsing->setCurrentIndex(0);
    }
    else
    {
        ui->comboBoxConnectUsing->setCurrentIndex(1);
    }

    ui->checkBoxExcludeVirtualPorts->setChecked(cd.ExcludeVirtualPorts);
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
    _connectionDetails.Type = _connType;
    _connectionDetails.ExcludeVirtualPorts = ui->checkBoxExcludeVirtualPorts->isChecked();
    if(_connectionDetails.Type == ConnectionType::Tcp)
    {
        if(!QHostAddress(ui->comboBoxIPAddress->currentText()).isNull())
        {
            _connectionDetails.TcpParams.IPAddress = ui->comboBoxIPAddress->currentText();
            _connectionDetails.TcpParams.ServicePort = ui->lineEditServicePort->value<int>();
            ui->comboBoxIPAddress->saveHistory();
            QFixedSizeDialog::accept();
        }
        else
        {
            QHostInfo::lookupHost(ui->comboBoxIPAddress->currentText(), this, [this](const QHostInfo &host)
            {
                if (host.error() != QHostInfo::NoError)
                {
                    QMessageBox::warning(this, parentWidget()->windowTitle(), tr("Lookup host failed: ") + host.errorString());
                    return;
                }

                _connectionDetails.TcpParams.IPAddress = ui->comboBoxIPAddress->currentText();
                _connectionDetails.TcpParams.ServicePort = ui->lineEditServicePort->value<int>();
                ui->comboBoxIPAddress->saveHistory();
                QFixedSizeDialog::accept();
            });
        }
    }
    else
    {
        _connectionDetails.SerialParams.PortName = ui->comboBoxSerialPort->currentPortName();
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
///
/// \brief DialogConnectionDetails::on_checkBoxExcludeVirtualPorts_checkStateChanged
/// \param state
///
void DialogConnectionDetails::on_checkBoxExcludeVirtualPorts_checkStateChanged(Qt::CheckState state)
{
    ui->comboBoxSerialPort->setExcludeVirtuals(state == Qt::Checked);
    validate();
}
#else
///
/// \brief DialogConnectionDetails::on_checkBoxExcludeVirtualPorts_stateChanged
/// \param state
///
void DialogConnectionDetails::on_checkBoxExcludeVirtualPorts_stateChanged(int state)
{
    ui->comboBoxSerialPort->setExcludeVirtuals(state == Qt::Checked);
    validate();
}
#endif

///
/// \brief DialogConnectionDetails::on_comboBoxIPAddress_currentTextChanged
///
void DialogConnectionDetails::on_comboBoxIPAddress_currentTextChanged(const QString&)
{
    validate();
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
void DialogConnectionDetails::on_comboBoxConnectUsing_currentIndexChanged(int index)
{   
    _connType = (index == 0) ? ConnectionType::Tcp : ConnectionType::Serial;
    ui->comboBoxIPAddress->setEnabled(_connType == ConnectionType::Tcp);
    ui->lineEditServicePort->setEnabled(_connType == ConnectionType::Tcp);
    ui->comboBoxBaudRate->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxParity->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxStopBits->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxWordLength->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxFlowControl->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxDTRControl->setEnabled(_connType == ConnectionType::Serial);

    const auto fc = ui->comboBoxFlowControl->currentFlowControl();
    ui->comboBoxRTSControl->setEnabled(_connType == ConnectionType::Serial && fc != QSerialPort::HardwareControl);

    ui->connectionDetails->setCurrentIndex(index);
    validate();
}

///
/// \brief DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged
///
void DialogConnectionDetails::on_comboBoxFlowControl_currentIndexChanged(int)
{
    const auto fc = ui->comboBoxFlowControl->currentFlowControl();
    ui->comboBoxDTRControl->setEnabled(_connType == ConnectionType::Serial);
    ui->comboBoxRTSControl->setEnabled(_connType == ConnectionType::Serial && fc != QSerialPort::HardwareControl);
}

///
/// \brief DialogConnectionDetails::validate
///
void DialogConnectionDetails::validate()
{
    switch(_connType)
    {
        case ConnectionType::Tcp:
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!ui->comboBoxIPAddress->currentText().isEmpty());
        break;

        case ConnectionType::Serial:
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->comboBoxSerialPort->count() > 0);
        break;
    }

}
