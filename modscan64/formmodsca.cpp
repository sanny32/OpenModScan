#include <QMessageBox>
#include "modbuslimits.h"
#include "modbusexception.h"
#include "mainwindow.h"
#include "formmodsca.h"
#include "ui_formmodsca.h"
#include "dialogwritecoilregister.h"
#include "dialogwriteholdingregister.h"

///
/// \brief FormModSca::FormModSca
/// \param num
/// \param parent
///
FormModSca::FormModSca(int num, QModbusClient* client, MainWindow* parent) :
    QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_modbusClient(client)
{
    Q_ASSERT(parent != nullptr);

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

    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);
    _timer.setInterval(1000);
    _timer.start();
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
    dd.ScanRate = _timer.interval();
    dd.DeviceId = ui->lineEditDeviceId->value<int>();
    dd.PointAddress = ui->lineEditAddress->value<int>();
    dd.PointType = ui->comboBoxModbusPointType->currentPointType();
    dd.Length = ui->lineEditLength->value<int>();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    _timer.setInterval(qBound(20U, dd.ScanRate, 10000U));
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
/// \brief FormModSca::on_readReply
///
void FormModSca::on_readReply()
{
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) return;

    ui->outputWidget->update(reply);

    if (reply->error() == QModbusDevice::NoError)
    {
        ui->statisticWidget->increaseValidSlaveResponses();
    }

    reply->deleteLater();
}

///
/// \brief FormModSca::on_writeReply
///
void FormModSca::on_writeReply()
{
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) return;

    if(reply->error() != QModbusDevice::NoError)
    {
        QString errorDesc;
        switch(reply->result().registerType())
        {
            case QModbusDataUnit::Coils:
                errorDesc = "Coil Write Failure";
            break;

            case QModbusDataUnit::HoldingRegisters:
                errorDesc = "Register Write Failure";
            break;

            default:
            break;
        }

        if (reply->error() == QModbusDevice::ProtocolError)
        {
            const QString exception = ModbusException(reply->rawResult().exceptionCode());
            const QString error = QString("%1. %2").arg(errorDesc, exception);
            QMessageBox::warning(this, windowTitle(), error);
        }
        else
        {
            QMessageBox::warning(this, windowTitle(), QString("%1. %2").arg(errorDesc, reply->errorString()));
        }
    }

    reply->deleteLater();
}

///
/// \brief createReadRequest
/// \param data
/// \return
///
QModbusRequest createReadRequest(const QModbusDataUnit& data)
{
    QModbusRequest request;
    switch (data.registerType())
    {
        case QModbusDataUnit::Coils:
            request = QModbusRequest(QModbusRequest::ReadCoils, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        case QModbusDataUnit::DiscreteInputs:
            request = QModbusRequest(QModbusRequest::ReadDiscreteInputs, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        case QModbusDataUnit::InputRegisters:
             request = QModbusRequest(QModbusRequest::ReadInputRegisters, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        case QModbusDataUnit::HoldingRegisters:
            request = QModbusRequest(QModbusRequest::ReadHoldingRegisters, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        default:
        break;
    }

    return request;
}

///
/// \brief FormModSca::sendReadRequest
///
void FormModSca::sendReadRequest()
{
    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    const auto dd = displayDefinition();
    const QModbusDataUnit dataUnit(dd.PointType, dd.PointAddress - 1, dd.Length);

    const auto request = createReadRequest(dataUnit);
    if(!request.isValid()) return;

    // update data
    ui->outputWidget->update(request);
    ui->statisticWidget->increaseNumberOfPolls();

    if(auto reply = _modbusClient->sendReadRequest(dataUnit, dd.DeviceId))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &FormModSca::on_readReply);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
}

///
/// \brief createWriteRequest
/// \param data
/// \return
///
QModbusRequest createWriteRequest(const QModbusDataUnit& data)
{
    QModbusRequest request;
    switch (data.registerType())
    {
        case QModbusDataUnit::Coils:
            request = QModbusRequest(QModbusRequest::WriteSingleCoil, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        case QModbusDataUnit::HoldingRegisters:
            request = QModbusRequest(QModbusRequest::WriteSingleRegister, quint16(data.startAddress()), quint16(data.valueCount()));
        break;
        default:
        break;
    }

    return request;
}

///
/// \brief createBoolDataUnit
/// \param newStartAddress
/// \param value
/// \return
///
QModbusDataUnit createBoolDataUnit(int newStartAddress, bool value)
{
    auto data = QModbusDataUnit(QModbusDataUnit::Coils, newStartAddress, 1);
    data.setValue(0, value);

    return data;
}

///
/// \brief createUIntDataUnit
/// \param newStartAddress
/// \param value
/// \return
///
QModbusDataUnit createUIntDataUnit(int newStartAddress, quint16 value)
{
    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 1);
    data.setValue(0, value);

    return data;
}

///
/// \brief createFloatDataUint
/// \param newStartAddress
/// \param value
/// \param inv
/// \return
///
QModbusDataUnit createFloatDataUint(int newStartAddress, float value, bool inv)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
    v.asFloat = value;

    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 2);
    if(inv)
    {
        data.setValue(0, v.asUint16[1]);
        data.setValue(1, v.asUint16[0]);
    }
    else
    {
        data.setValue(0, v.asUint16[0]);
        data.setValue(1, v.asUint16[1]);
    }

    return data;
}

///
/// \brief createDoubleDataUint
/// \param newStartAddress
/// \param value
/// \param inv
/// \return
///
QModbusDataUnit createDoubleDataUint(int newStartAddress, double value, bool inv)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asDouble = value;

    auto data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, newStartAddress, 4);
    if(inv)
    {
        data.setValue(0, v.asUint16[3]);
        data.setValue(1, v.asUint16[2]);
        data.setValue(2, v.asUint16[1]);
        data.setValue(3, v.asUint16[0]);
    }
    else
    {
        data.setValue(0, v.asUint16[0]);
        data.setValue(1, v.asUint16[1]);
        data.setValue(2, v.asUint16[2]);
        data.setValue(3, v.asUint16[3]);
    }

    return data;
}

///
/// \brief FormModSca::writeRegister
/// \param pointType
/// \param params
///
void FormModSca::writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params)
{
    QModbusDataUnit data;
    switch (pointType)
    {
        case QModbusDataUnit::Coils:
            data = createBoolDataUnit(params.Address - 1, params.Value.toBool());
        break;

        case QModbusDataUnit::HoldingRegisters:
            switch(ui->outputWidget->dataDisplayMode())
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Decimal:
                case DataDisplayMode::Integer:
                case DataDisplayMode::Hex:
                    data = createUIntDataUnit(params.Address - 1, params.Value.toUInt());
                break;

                case DataDisplayMode::FloatingPt:
                    data = createFloatDataUint(params.Address - 1, params.Value.toFloat(), false);
                break;

                case DataDisplayMode::SwappedFP:
                    data = createFloatDataUint(params.Address - 1, params.Value.toFloat(), true);
                break;

                case DataDisplayMode::DblFloat:
                    data = createDoubleDataUint(params.Address - 1, params.Value.toDouble(), false);
                break;

                case DataDisplayMode::SwappedDbl:
                    data = createDoubleDataUint(params.Address - 1, params.Value.toDouble(), true);
                break;
            }
        break;

        default:
        break;
    }

    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
    {
        QString errorDesc;
        switch(pointType)
        {
            case QModbusDataUnit::Coils:
                errorDesc = "Coil Write Failure";
            break;

            case QModbusDataUnit::HoldingRegisters:
                errorDesc = "Register Write Failure";
            break;

            default:
            break;
        }

        QMessageBox::warning(this, windowTitle(), errorDesc);
        return;
    }

    const auto request = createWriteRequest(data);
    if(!request.isValid()) return;

    // update data
    ui->outputWidget->update(request);

    if(auto reply = _modbusClient->sendWriteRequest(data, params.Node))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &FormModSca::on_writeReply);
        }
        else
        {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    }
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{
    if(_modbusClient == nullptr)
    {
        return;
    }

    if(_modbusClient->state() != QModbusDevice::ConnectedState)
    {
        ui->outputWidget->setStatus("Device NOT CONNECTED!");
        return;
    }

    const auto dd = displayDefinition();
    if(dd.PointAddress + dd.Length - 1 > ModbusLimits::addressRange().to())
    {
        ui->outputWidget->setStatus("Invalid Data Length Specified");
        return;
    }

    sendReadRequest();
}

///
/// \brief FormModSca::on_lineEditAddress_valueChanged
///
void FormModSca::on_lineEditAddress_valueChanged(const QVariant&)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditLength_valueChanged
///
void FormModSca::on_lineEditLength_valueChanged(const QVariant&)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_lineEditDeviceId_valueChanged
///
void FormModSca::on_lineEditDeviceId_valueChanged(const QVariant&)
{
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_pointTypeChanged
///
void FormModSca::on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType)
{
    ui->outputWidget->setup(displayDefinition());
}

///
/// \brief FormModSca::on_outputWidget_itemDoubleClicked
/// \param addr
/// \param value
///
void FormModSca::on_outputWidget_itemDoubleClicked(quint32 addr, const QVariant& value)
{
    const quint32 node = ui->lineEditDeviceId->value<int>();
    const auto pointType = ui->comboBoxModbusPointType->currentPointType();
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        {
            ModbusWriteParams params = { node, addr, value };
            DialogWriteCoilRegister dlg(params, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                writeRegister(pointType, params);
            }
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            ModbusWriteParams params = { node, addr, value };
            DialogWriteHoldingRegister dlg(params, dataDisplayMode(), this);
            if(dlg.exec() == QDialog::Accepted)
            {
                writeRegister(pointType, params);
            }
        }
        break;

        default:
        break;
    }
}
