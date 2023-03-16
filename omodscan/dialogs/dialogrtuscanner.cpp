#include <QDateTime>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QAbstractEventDispatcher>
#include "dialogrtuscanner.h"
#include "ui_dialogrtuscanner.h"

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
        return DialogRtuScanner::tr("NONE");

        case QSerialPort::EvenParity:
        return DialogRtuScanner::tr("EVEN");

        case QSerialPort::OddParity:
        return DialogRtuScanner::tr("ODD");

        case QSerialPort::SpaceParity:
        return DialogRtuScanner::tr("SPACE");

        case QSerialPort::MarkParity:
        return DialogRtuScanner::tr("MARK");

        default:
        break;
    }

    return QString();
}

///
/// \brief DialogRtuScanner::DialogRtuScanner
/// \param parent
///
DialogRtuScanner::DialogRtuScanner(QWidget *parent)
    : QFixedSizeDialog(parent)
    , ui(new Ui::DialogRtuScanner)
    ,_modbusClient(new QModbusRtuSerialClient(this))
{
    ui->setupUi(this);

    for(auto&& port: QSerialPortInfo::availablePorts())
    {
        ui->comboBoxSerial->addItem(port.portName());
    }

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogRtuScanner::on_awake);

    connect(&_scanTimer, &QTimer::timeout, this, &DialogRtuScanner::on_timeout);
    connect(_modbusClient, &QModbusClient::stateChanged, this, &DialogRtuScanner::on_stateChanged);
    connect(_modbusClient, &QModbusClient::errorOccurred, this, &DialogRtuScanner::on_errorOccurred);
}

///
/// \brief DialogRtuScanner::~DialogRtuScanner
///
DialogRtuScanner::~DialogRtuScanner()
{
    delete ui;
    delete _modbusClient;
}

///
/// \brief DialogRtuScanner::on_awake
///
void DialogRtuScanner::on_awake()
{
    ui->comboBoxSerial->setEnabled(!_scanning);
    ui->groupBoxSpeed->setEnabled(!_scanning);
    ui->groupBoxDataBits->setEnabled(!_scanning);
    ui->groupBoxParity->setEnabled(!_scanning);
    ui->groupBoxAddress->setEnabled(!_scanning);
    ui->groupBoxStopBits->setEnabled(!_scanning);
    ui->groupBoxTimeoute->setEnabled(!_scanning);
    ui->pushButtonScan->setText(_scanning ? tr("Stop Scan") : tr("Start Scan"));
}

///
/// \brief DialogRtuScanner::on_timeout
///
void DialogRtuScanner::on_timeout()
{
    setScanTme(_scanTime + 1);
}

///
/// \brief DialogRtuScanner::on_pushButtonScan_clicked
///
void DialogRtuScanner::on_pushButtonScan_clicked()
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
void DialogRtuScanner::on_pushButtonClear_clicked()
{
    ui->listWidget->clear();
}

///
/// \brief DialogRtuScanner::on_errorOccurred
/// \param error
///
void DialogRtuScanner::on_errorOccurred(QModbusDevice::Error error)
{
    if(error == QModbusDevice::ConnectionError &&
       _modbusClient->state() == QModbusDevice::ConnectingState)
    {
        stopScan();
        QMessageBox::warning(this, windowTitle(), _modbusClient->errorString());
    }
}

///
/// \brief DialogRtuScanner::on_stateChanged
/// \param state
///
void DialogRtuScanner::on_stateChanged(QModbusDevice::State state)
{
   if(state == QModbusDevice::ConnectedState)
        sendRequest(ui->spinBoxAddressFrom->value());
}

///
/// \brief DialogRtuScanner::startScan
///
void DialogRtuScanner::startScan()
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
void DialogRtuScanner::stopScan()
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
void DialogRtuScanner::clearScanTime()
{
    setScanTme(0);
}

///
/// \brief DialogRtuScanner::clearProgress
///
void DialogRtuScanner::clearProgress()
{
    ui->progressBar->setValue(0);
}

///
/// \brief DialogRtuScanner::printScanInfo
/// \param params
/// \param address
///
void DialogRtuScanner::printScanInfo(const SerialConnectionParams& params, int address)
{
    ui->labelSpeed->setText(QString(tr("Speed: %1")).arg(params.BaudRate));
    ui->labelDataBits->setText(QString(tr("Data Bits: %1")).arg(params.WordLength));
    ui->labelParity->setText(QString(tr("Parity: %1")).arg(Parity_toString(params.Parity)));
    ui->labelStopBits->setText(QString(tr("Stop Bits: %1")).arg(params.StopBits));
    ui->labelAddress->setText(QString(tr("Address: %1")).arg(address));

    const double total = _connParams.size() * (ui->spinBoxAddressTo->value() - ui->spinBoxAddressFrom->value() + 1);
    const double progress = ((std::distance(_connParams.cbegin(), _iterator) + 1) + (address - ui->spinBoxAddressFrom->value() + 1)) / total;
    ui->progressBar->setValue(progress * 100);
}

///
/// \brief DialogRtuScanner::printResult
/// \param params
/// \param address
///
void DialogRtuScanner::printResult(const SerialConnectionParams& params, int address)
{
    const auto result = QString("%1: %2 (%3,%4,%5,%6)").arg(params.PortName,
                                                           QString::number(address),
                                                           QString::number(params.BaudRate),
                                                           QString::number(params.WordLength),
                                                           Parity_toString(params.Parity),
                                                           QString::number(params.StopBits));
    ui->listWidget->addItem(result);
}

///
/// \brief DialogRtuScanner::prepareParams
///
void DialogRtuScanner::prepareParams()
{
    _connParams.clear();

    QVector<QSerialPort::BaudRate> baudRates;
    for(auto&& obj : ui->groupBoxSpeed->children())
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
                    if(!_connParams.contains(params))
                        _connParams.push_back(params);
                }
            }
        }
    }

    // normalize modbus address
    if(ui->spinBoxAddressFrom->value() > ui->spinBoxAddressTo->value())
    {
        const auto tmp = ui->spinBoxAddressFrom->value();
        ui->spinBoxAddressFrom->setValue(ui->spinBoxAddressTo->value());
        ui->spinBoxAddressTo->setValue(tmp);
    }
}

///
/// \brief DialogRtuScanner::setScanTme
/// \param time
///
void DialogRtuScanner::setScanTme(quint64 time)
{
    _scanTime = qMax(0U, time);
    const auto str = QDateTime::fromSecsSinceEpoch(_scanTime).toUTC().toString("hh:mm:ss");
    ui->labelTimeLeft->setText(QString("<html><head/><body><p><span style=\"font-weight:700;\">%1</span></p></body></html>").arg(str));
}

///
/// \brief DialogRtuScanner::connectDevice
/// \param params
///
void DialogRtuScanner::connectDevice(const SerialConnectionParams& params)
{
    _modbusClient->disconnectDevice();
    _modbusClient->setNumberOfRetries(1);
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
/// \param address
///
void DialogRtuScanner::sendRequest(int address)
{
    if(!_scanning)
        return;

    if(address > ui->spinBoxAddressTo->value())
    {
        _iterator++;
        if(_iterator != _connParams.end())
            connectDevice(*_iterator);
        else
            stopScan();

        return;
    }

    printScanInfo(*_iterator, address);

    QModbusRequest req(QModbusRequest::ReadHoldingRegisters, quint16(1), quint16(1));
    if(auto reply = _modbusClient->sendRawRequest(req, address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, address]()
            {
                if(reply->error() != QModbusDevice::TimeoutError &&
                   reply->error() != QModbusDevice::ConnectionError &&
                   reply->error() != QModbusDevice::ReplyAbortedError)
                {
                    printResult(*_iterator, address);
                }
                reply->deleteLater();

                sendRequest(address + 1);
            },
            Qt::QueuedConnection);
        }
        else
        {
            delete reply; // broadcast replies return immediately
            sendRequest(address + 1);
        }
    }
    else
    {
        sendRequest(address + 1);
    }
}
