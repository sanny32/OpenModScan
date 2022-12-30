#include "modbuslimits.h"
#include "mainwindow.h"
#include "formmodsca.h"
#include "ui_formmodsca.h"

///
/// \brief FormModSca::FormModSca
/// \param num
/// \param parent
///
FormModSca::FormModSca(int num, QModbusClient* client, MainWindow* parent) :
    QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_modbusClient(client)
    ,_scanRate(1000)
{
    ui->setupUi(this);
    setWindowTitle(QString("ModSca%1").arg(num));

    ui->lineEditAddress->setPaddingZeroes(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditAddress->setValue(1);

    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditLength->setValue(50);

    ui->lineEditDeviceId->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditDeviceId->setValue(1);

    ui->outputWidget->setup(displayDefinition());

    connect(parent, &MainWindow::modbusClientChanged, this,
            [&](QModbusClient* cli)
            {
                _modbusClient = cli;
            });

    QTimer::singleShot(_scanRate, this, [&] { sendReadRequest(); });
}

///
/// \brief FormModSca::~FormModSca
///
FormModSca::~FormModSca()
{
    delete ui;
}

///
/// \brief FormModSca::displayDefinition
/// \return
///
DisplayDefinition FormModSca::displayDefinition() const
{
    DisplayDefinition dd;
    dd.ScanRate = _scanRate;
    dd.DeviceId = ui->lineEditDeviceId->value();
    dd.PointAddress = ui->lineEditAddress->value();
    dd.PointType = ui->comboBoxModbusPointType->currentPointType();
    dd.Length = ui->lineEditLength->value();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    _scanRate = dd.ScanRate;
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);

    ui->outputWidget->setStatus("Data Uninitialized");
    ui->outputWidget->setup(dd);
}

///
/// \brief FormModSca::displayMode
/// \return
///
DisplayMode FormModSca::displayMode() const
{
    return ui->outputWidget->displayMode();
}

///
/// \brief FormModSca::setDisplayMode
/// \param mode
///
void FormModSca::setDisplayMode(DisplayMode mode)
{
    ui->outputWidget->setDisplayMode(mode);
}

///
/// \brief FormModSca::dataDisplayMode
/// \return
///
DataDisplayMode FormModSca::dataDisplayMode() const
{
    return ui->outputWidget->dataDisplayMode();
}

///
/// \brief FormModSca::setDataDisplayMode
/// \param mode
///
void FormModSca::setDataDisplayMode(DataDisplayMode mode)
{
    ui->outputWidget->setDataDisplayMode(mode);
}

///
/// \brief FormModSca::resetCtrls
///
void FormModSca::resetCtrs()
{
    ui->statisticWidget->resetCtrs();
}

///
/// \brief FormModSca::readyReadData
///
void FormModSca::readyReadData()
{
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) return;

    ui->outputWidget->update(reply);

    if (reply->error() == QModbusDevice::NoError)
    {
        ui->statisticWidget->increaseValidSlaveResponses();

        const uint delay = _modbusClient->property("DelayBetweenPolls").toUInt();
        QTimer::singleShot(delay + _scanRate, this, [&] { sendReadRequest(); });
    }
    else
    {
        QTimer::singleShot(_scanRate, this, [&] { sendReadRequest(); });
    }

    reply->deleteLater();
}

///
/// \brief FormModSca::sendReadRequest
///
void FormModSca::sendReadRequest()
{
    try
    {
        if(_modbusClient == nullptr)
        {
            QTimer::singleShot(_scanRate, this, [&] { sendReadRequest(); });
            return;
        }

        if(_modbusClient->state() != QModbusDevice::ConnectedState)
        {
            throw QString("Device NOT CONNECTED!");
        }

        const auto dd = displayDefinition();
        if(dd.PointAddress + dd.Length - 1 > ModbusLimits::addressRange().to())
        {
            throw QString("Invalid Data Length Specified");
        }

        QModbusRequest request;
        switch (dd.PointType)
        {
            case QModbusDataUnit::Coils:
                request = QModbusRequest(QModbusRequest::ReadCoils, quint16(dd.PointAddress - 1), dd.Length);
            break;
            case QModbusDataUnit::DiscreteInputs:
                request = QModbusRequest(QModbusRequest::ReadDiscreteInputs, quint16(dd.PointAddress - 1), dd.Length);
            break;
            case QModbusDataUnit::InputRegisters:
                 request = QModbusRequest(QModbusRequest::ReadInputRegisters, quint16(dd.PointAddress - 1), dd.Length);
            break;
            case QModbusDataUnit::HoldingRegisters:
                request = QModbusRequest(QModbusRequest::ReadHoldingRegisters, quint16(dd.PointAddress - 1), dd.Length);
            break;
            default:
            break;
        }

        // update data
        ui->outputWidget->update(request);
        ui->statisticWidget->increaseNumberOfPolls();

        // modbus request
        QModbusDataUnit dataUnit(dd.PointType, dd.PointAddress - 1, dd.Length);
        auto reply = _modbusClient->sendReadRequest(dataUnit, dd.DeviceId);
        if(!reply)
        {
            throw _modbusClient->errorString();
        }

        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &FormModSca::readyReadData);
        }
        else
        {
            delete reply; // broadcast replies return immediately
            QTimer::singleShot(_scanRate, this, [&] { sendReadRequest(); });
        }
    }
    catch(const QString& err)
    {
        ui->outputWidget->setStatus(err);
        QTimer::singleShot(_scanRate, this, [&] { sendReadRequest(); });
    }
}

///
/// \brief FormModSca::on_lineEditAddress_valueChanged
///
void FormModSca::on_lineEditAddress_valueChanged(int)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditLength_valueChanged
///
void FormModSca::on_lineEditLength_valueChanged(int)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditDeviceId_valueChanged
///
void FormModSca::on_lineEditDeviceId_valueChanged(int)
{
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_currentTextChanged
///
void FormModSca::on_comboBoxModbusPointType_currentTextChanged(const QString&)
{
    ui->outputWidget->setup(displayDefinition());
}
