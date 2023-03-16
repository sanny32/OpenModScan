#include <QDateTime>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QAbstractEventDispatcher>
#include "dialogrtuscanner.h"
#include "ui_dialogrtuscanner.h"

QString Parity_toString(QSerialPort::Parity parity)
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
    ui->pushButtonScan->setText(_scanning ? tr("Stop Scan") : tr("Start Scan"));
}

///
/// \brief DialogRtuScanner::on_timeout
///
void DialogRtuScanner::on_timeout()
{
    const auto elapsed = QDateTime::fromSecsSinceEpoch(_scanTime++).toUTC().toString("hh:mm:ss");
    ui->labelTimeLeft->setText(QString("<html><head/><body><p><span style=\"font-weight:700;\">%1</span></p></body></html>").arg(elapsed));
}


///
/// \brief DialogRtuScanner::on_pushButtonScan_clicked
///
void DialogRtuScanner::on_pushButtonScan_clicked()
{
    if(!_scanning)
        startScan();
    else
        stopScan();
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
    if(error == QModbusDevice::ConnectionError)
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
   {
        const auto fromAddres = ui->spinBoxAddressFrom->value();
        const auto toAddress = ui->spinBoxAddressTo->value();
        for(int i = fromAddres; i <= toAddress; i++)
            sendRequest(i);
   }
}

///
/// \brief DialogRtuScanner::startScan
///
void DialogRtuScanner::startScan()
{
    if(_scanning)
        return;

    _scanning = true;
    _finished = false;

    clearScanTime();
    clearProgress();

    _iterator = _connParams.cbegin();
    connectDevice(*_iterator);

    _scanTimer.start(1000);
}

///
/// \brief DialogRtuScanner::stopScan
///
void DialogRtuScanner::stopScan()
{
    if(!_scanning)
        return;

    _scanning = false;
    _finished = true;
    _scanTimer.stop();
}

///
/// \brief DialogRtuScanner::clearScanTime
///
void DialogRtuScanner::clearScanTime()
{
    _scanTime = 0;
    ui->labelTimeLeft->setText("<html><head/><body><p><span style=\"font-weight:700;\">00:00:00</span></p></body></html>");
}

///
/// \brief DialogRtuScanner::clearProgress
///
void DialogRtuScanner::clearProgress()
{
    _requestCount = 0;
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
}

///
/// \brief DialogRtuScanner::printResult
/// \param params
/// \param address
///
void DialogRtuScanner::printResult(const SerialConnectionParams& params, int address)
{
    const auto result = QString("%1: %2 (%3,%4,%5,%6").arg(params.PortName,
                                                           QString::number(address),
                                                           QString::number(params.BaudRate),
                                                           QString::number(params.WordLength),
                                                           Parity_toString(params.Parity),
                                                           QString::number(params.StopBits));
    ui->listWidget->addItem(result);
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
    printScanInfo(*_iterator, address);

    QModbusRequest req(QModbusRequest::ReadHoldingRegisters, quint16(1), quint16(1));
    if(auto reply = _modbusClient->sendRawRequest(req, address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply, address]()
            {
                if(reply->error() != QModbusDevice::TimeoutError &&
                   reply->error() != QModbusDevice::ConnectionError)
                {
                    printResult(*_iterator, address);
                }
                reply->deleteLater();

                connectDevice(*++_iterator);
            });
        }
        else
        {
            delete reply; // broadcast replies return immediately
            connectDevice(*++_iterator);
        }
    }
    else
    {
        connectDevice(*++_iterator);
    }
}
