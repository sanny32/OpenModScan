#include <QDateTime>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QAbstractEventDispatcher>
#include "dialogmodbusscanner.h"
#include "ui_dialogmodbusscanner.h"

///
/// \brief Parity_toString
/// \param parity
/// \return
///
inline QString Parity_toString(QSerialPort::Parity parity)
{
    switch(parity)
    {
        case QSerialPort::NoParity:
        return DialogModbusScanner::tr("None");

        case QSerialPort::EvenParity:
        return DialogModbusScanner::tr("Even");

        case QSerialPort::OddParity:
        return DialogModbusScanner::tr("Odd");

        case QSerialPort::SpaceParity:
        return DialogModbusScanner::tr("Space");

        case QSerialPort::MarkParity:
        return DialogModbusScanner::tr("Mark");

        default:
        break;
    }

    return QString();
}

///
/// \brief DialogRtuScanner::DialogRtuScanner
/// \param parent
///
DialogModbusScanner::DialogModbusScanner(QWidget *parent)
    : QFixedSizeDialog(parent)
    , ui(new Ui::DialogModbusScanner)
    ,_modbusClient(new QModbusRtuSerialClient(this))
{
    ui->setupUi(this);
    ui->progressBar->setAlignment(Qt::AlignCenter);

    for(auto&& port: QSerialPortInfo::availablePorts())
        ui->comboBoxSerial->addItem(port.portName());

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogModbusScanner::on_awake);

    connect(&_scanTimer, &QTimer::timeout, this, &DialogModbusScanner::on_timeout);
    connect(_modbusClient, &QModbusClient::stateChanged, this, &DialogModbusScanner::on_stateChanged);
    connect(_modbusClient, &QModbusClient::errorOccurred, this, &DialogModbusScanner::on_errorOccurred);

    auto serialPort = qobject_cast<QSerialPort*>(_modbusClient->device());
    QObject::connect(serialPort, &QSerialPort::readyRead, this,
    [this]()
    {
        printResult(*_iterator, _modbusClient->property("DeviceId").toInt());
    });
}

///
/// \brief DialogRtuScanner::~DialogRtuScanner
///
DialogModbusScanner::~DialogModbusScanner()
{
    delete ui;
    delete _modbusClient;
}

///
/// \brief DialogRtuScanner::changeEvent
/// \param event
///
void DialogModbusScanner::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);

    QDialog::changeEvent(event);
}

///
/// \brief DialogModbusScanner::showEvent
/// \param e
///
void DialogModbusScanner::showEvent(QShowEvent* e)
{
    QFixedSizeDialog::showEvent(e);
    ui->radioButtonRTU->click();
}

///
/// \brief DialogRtuScanner::on_awake
///
void DialogModbusScanner::on_awake()
{
    const bool rtuScanning = ui->radioButtonRTU->isChecked();
    ui->comboBoxSerial->setEnabled(!_scanning && rtuScanning);
    ui->groupBoxBaudRate->setEnabled(!_scanning && rtuScanning);
    ui->groupBoxDataBits->setEnabled(!_scanning && rtuScanning);
    ui->groupBoxParity->setEnabled(!_scanning && rtuScanning);
    ui->groupBoxStopBits->setEnabled(!_scanning && rtuScanning);
    ui->groupBoxDeviceId->setEnabled(!_scanning);
    ui->groupBoxTimeoute->setEnabled(!_scanning);
    ui->pushButtonClear->setEnabled(!_scanning);
    ui->pushButtonScan->setEnabled(ui->comboBoxSerial->count() > 0);
    ui->pushButtonScan->setText(_scanning ? tr("Stop Scan") : tr("Start Scan"));
}

///
/// \brief DialogRtuScanner::on_timeout
///
void DialogModbusScanner::on_timeout()
{
    setScanTme(_scanTime + 1);
}

///
/// \brief DialogRtuScanner::on_pushButtonScan_clicked
///
void DialogModbusScanner::on_pushButtonScan_clicked()
{
    setCursor(Qt::WaitCursor);

    if(!_scanning)
        startScan();
    else
        stopScan();

    setCursor(Qt::ArrowCursor);
}

///
/// \brief DialogRtuScanner::on_pushButtonClear_clicked
///
void DialogModbusScanner::on_pushButtonClear_clicked()
{
    ui->listWidget->clear();
}

///
/// \brief DialogRtuScanner::on_errorOccurred
/// \param error
///
void DialogModbusScanner::on_errorOccurred(QModbusDevice::Error error)
{
    if(error == QModbusDevice::ConnectionError &&
       _modbusClient->state() == QModbusDevice::ConnectingState)
    {
        const auto errorString = _modbusClient->errorString();

        stopScan();
        QMessageBox::warning(this, windowTitle(), errorString);
    }
}

///
/// \brief DialogRtuScanner::on_stateChanged
/// \param state
///
void DialogModbusScanner::on_stateChanged(QModbusDevice::State state)
{
   if(state == QModbusDevice::ConnectedState)
        sendRequest(ui->spinBoxDeviceIdFrom->value());
}

///
/// \brief DialogRtuScanner::on_listWidget_itemDoubleClicked
/// \param item
///
void DialogModbusScanner::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    stopScan();

    const auto deviceId = item->data(Qt::UserRole + 1).toInt();
    const auto params = item->data(Qt::UserRole).value<SerialConnectionParams>();

    emit attemptToConnect(params, deviceId);
    close();
}

///
/// \brief DialogModbusScanner::on_radioButtonRTU_clicked
///
void DialogModbusScanner::on_radioButtonRTU_clicked()
{
    ui->groupBoxIPAddressRange->setVisible(false);
    ui->groupBoxPort->setVisible(true);
    ui->labelScanResultsDesc->setText("PORT: Device Id (serial port settings)");
}

///
/// \brief DialogModbusScanner::on_radioButtonTCP_clicked
///
void DialogModbusScanner::on_radioButtonTCP_clicked()
{
    ui->groupBoxIPAddressRange->setVisible(true);
    ui->groupBoxPort->setVisible(false);
    ui->labelScanResultsDesc->setText("IP Address: Device Id");
}

///
/// \brief DialogRtuScanner::startScan
///
void DialogModbusScanner::startScan()
{
    if(_scanning)
        return;

    clearScanTime();
    clearProgress();
    prepareParams();

    _scanning = true;
    _scanTimer.start(1000);

    if(_connParams.empty())
    {
        stopScan();
        return;
    }

    _iterator = _connParams.cbegin();
    connectDevice(*_iterator);
}

///
/// \brief DialogRtuScanner::stopScan
///
void DialogModbusScanner::stopScan()
{
    if(!_scanning)
        return;

    _modbusClient->disconnectDevice();

    _scanning = false;
    _scanTimer.stop();
}

///
/// \brief DialogRtuScanner::clearScanTime
///
void DialogModbusScanner::clearScanTime()
{
    setScanTme(0);
}

///
/// \brief DialogRtuScanner::clearProgress
///
void DialogModbusScanner::clearProgress()
{
    ui->progressBar->setValue(0);
}

///
/// \brief DialogRtuScanner::printScanInfo
/// \param params
/// \param deviceId
///
void DialogModbusScanner::printScanInfo(const SerialConnectionParams& params, int deviceId)
{
    ui->labelSpeed->setText(QString(tr("Baud Rate: %1")).arg(params.BaudRate));
    ui->labelDataBits->setText(QString(tr("Data Bits: %1")).arg(params.WordLength));
    ui->labelParity->setText(QString(tr("Parity: %1")).arg(Parity_toString(params.Parity)));
    ui->labelStopBits->setText(QString(tr("Stop Bits: %1")).arg(params.StopBits));
    ui->labelAddress->setText(QString(tr("Device Id: %1")).arg(deviceId));

    const double size = _connParams.size();
    const double addrLen = (ui->spinBoxDeviceIdTo->value() - ui->spinBoxDeviceIdFrom->value() + 1);
    const double total = size * addrLen;
    const double progress = std::distance(_connParams.cbegin(), _iterator) / size  + (deviceId - ui->spinBoxDeviceIdFrom->value() + 1) / total;
    ui->progressBar->setValue(progress * 100);
}

///
/// \brief DialogRtuScanner::printResult
/// \param params
/// \param deviceId
///
void DialogModbusScanner::printResult(const SerialConnectionParams& params, int deviceId)
{
    const auto result = QString("%1: %2 (%3,%4,%5,%6)").arg(params.PortName,
                                                           QString::number(deviceId),
                                                           QString::number(params.BaudRate),
                                                           QString::number(params.WordLength),
                                                           Parity_toString(params.Parity),
                                                           QString::number(params.StopBits));

    const auto items = ui->listWidget->findItems(result, Qt::MatchExactly);
    if(items.empty())
    {
        auto item = new QListWidgetItem(ui->listWidget);
        item->setText(result);
        item->setData(Qt::UserRole, QVariant::fromValue(params));
        item->setData(Qt::UserRole + 1, deviceId);

        ui->listWidget->addItem(item);
    }
}

///
/// \brief DialogRtuScanner::prepareParams
///
void DialogModbusScanner::prepareParams()
{
    _connParams.clear();

    QVector<QSerialPort::BaudRate> baudRates;
    for(auto&& obj : ui->groupBoxBaudRate->children())
    {
        auto chbx = qobject_cast<QCheckBox*>(obj);
        if(chbx && chbx->isChecked())
            baudRates.push_back((QSerialPort::BaudRate)chbx->text().toInt());
    }

    QVector<QSerialPort::DataBits> dataBits;
    for(auto&& obj: ui->groupBoxDataBits->children())
    {
        auto chbx = qobject_cast<QCheckBox*>(obj);
        if(chbx && chbx->isChecked())
            dataBits.push_back((QSerialPort::DataBits)chbx->text().toInt());
    }

    QVector<QSerialPort::Parity> parities;
    if(ui->checkBoxParityNone->isChecked())
        parities.push_back(QSerialPort::NoParity);
    if(ui->checkBoxParityOdd->isChecked())
        parities.push_back(QSerialPort::OddParity);
    if(ui->checkBoxParityEven->isChecked())
        parities.push_back(QSerialPort::EvenParity);

    QVector<QSerialPort::StopBits> stopBits;
    for(auto&& obj: ui->groupBoxStopBits->children())
    {
        auto chbx = qobject_cast<QCheckBox*>(obj);
        if(chbx && chbx->isChecked())
            stopBits.push_back((QSerialPort::StopBits)chbx->text().toInt());
    }

    for(auto&& baudRate : baudRates)
    {
        for(auto&& wordLength : dataBits)
        {
            for(auto&& parity : parities)
            {
                for(auto&& stop : stopBits)
                {
                    SerialConnectionParams params;
                    params.PortName = ui->comboBoxSerial->currentText();
                    params.BaudRate = baudRate;
                    params.WordLength = wordLength;
                    params.Parity = parity;
                    params.StopBits = stop;
                    _connParams.append(params);
                }
            }
        }
    }

    // remove duplicates
    _connParams.erase(std::unique(_connParams.begin(), _connParams.end()), _connParams.end());

    // normalize modbus address
    if(ui->spinBoxDeviceIdFrom->value() > ui->spinBoxDeviceIdTo->value())
    {
        const auto tmp = ui->spinBoxDeviceIdFrom->value();
        ui->spinBoxDeviceIdFrom->setValue(ui->spinBoxDeviceIdTo->value());
        ui->spinBoxDeviceIdTo->setValue(tmp);
    }
}

///
/// \brief DialogRtuScanner::setScanTme
/// \param time
///
void DialogModbusScanner::setScanTme(quint64 time)
{
    _scanTime = time;
    const auto str = QDateTime::fromSecsSinceEpoch(_scanTime).toUTC().toString("hh:mm:ss");
    ui->labelTimeLeft->setText(QString("<html><head/><body><p><span style=\"font-weight:700;\">%1</span></p></body></html>").arg(str));
}

///
/// \brief DialogRtuScanner::connectDevice
/// \param params
///
void DialogModbusScanner::connectDevice(const SerialConnectionParams& params)
{
    _modbusClient->disconnectDevice();
    _modbusClient->setNumberOfRetries(0);
    _modbusClient->setTimeout(ui->spinBoxTimeout->value());
    _modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, params.PortName);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, params.Parity);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, params.BaudRate);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, params.WordLength);
    _modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, params.StopBits);
    _modbusClient->connectDevice();
}

///
/// \brief DialogRtuScanner::sendRequest
/// \param deviceId
///
void DialogModbusScanner::sendRequest(int deviceId)
{
    if(!_scanning)
        return;

    if(deviceId > ui->spinBoxDeviceIdTo->value())
    {
        _iterator++;

        if(_iterator != _connParams.end())
            connectDevice(*_iterator);
        else
            stopScan();

        return;
    }

    printScanInfo(*_iterator, deviceId);
    _modbusClient->setProperty("DeviceId", deviceId);

    QModbusRequest req(QModbusPdu::ReportServerId);
    if(auto reply = _modbusClient->sendRawRequest(req, deviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, deviceId]()
            {
                if(reply->error() != QModbusDevice::TimeoutError &&
                   reply->error() != QModbusDevice::ConnectionError &&
                   reply->error() != QModbusDevice::ReplyAbortedError)
                {
                    printResult(*_iterator, deviceId);
                }
                reply->deleteLater();

                sendRequest(deviceId + 1);
            },
            Qt::QueuedConnection);
        }
        else
        {
            delete reply; // broadcast replies return immediately
            sendRequest(deviceId + 1);
        }
    }
    else
    {
        sendRequest(deviceId + 1);
    }
}
