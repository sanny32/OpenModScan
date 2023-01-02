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
/// \brief FormModSca::createReadRequest
/// \return
///
QModbusRequest FormModSca::createReadRequest()
{
    QModbusRequest request;
    const auto dd = displayDefinition();

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

    return request;
}

///
/// \brief FormModSca::sendReadRequest
/// \param request
/// \param id
///
void FormModSca::sendReadRequest(const QModbusRequest& request, uint id)
{
    if(!request.isValid())
    {
        return;
    }

    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    const auto dd = displayDefinition();
    const QModbusDataUnit dataUnit(dd.PointType, dd.PointAddress - 1, dd.Length);
    auto reply = _modbusClient->sendReadRequest(dataUnit, id);
    if(!reply) return;

    if (!reply->isFinished())
    {
        connect(reply, &QModbusReply::finished, this, &FormModSca::on_readReply);
    }
    else
    {
        delete reply; // broadcast replies return immediately
    }
}

///
/// \brief FormModSca::writeRegister
/// \param pointType
/// \param params
///
void FormModSca::writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params)
{
    QString errorDesc;
    QModbusDataUnit data;
    switch (pointType)
    {
        case QModbusDataUnit::Coils:
            errorDesc = "Coil Write Failure";
            data = QModbusDataUnit(QModbusDataUnit::Coils, params.Address - 1, 1);
            data.setValue(0, params.Value.toBool());
        break;

        case QModbusDataUnit::HoldingRegisters:
            errorDesc = "Register Write Failure";
            switch(ui->outputWidget->dataDisplayMode())
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Decimal:
                case DataDisplayMode::Hex:
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 1);
                    data.setValue(0, params.Value.toUInt());
                break;

                case DataDisplayMode::Integer:
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 1);
                    data.setValue(0, params.Value.toInt());
                break;

                case DataDisplayMode::FloatingPt:
                {
                    union {
                       quint16 asUint16[2];
                       float asFloat;
                    } v;
                    v.asFloat = params.Value.toFloat();
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 2);
                    data.setValue(0, v.asUint16[0]);
                    data.setValue(1, v.asUint16[1]);
                }
                break;

                case DataDisplayMode::SwappedFP:
                {
                    union {
                       quint16 asUint16[2];
                       float asFloat;
                    } v;
                    v.asFloat = params.Value.toFloat();
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 2);
                    data.setValue(0, v.asUint16[1]);
                    data.setValue(1, v.asUint16[0]);
                }
                break;

                case DataDisplayMode::DblFloat:
                {
                    union {
                       quint16 asUint16[4];
                       double asDouble;
                    } v;
                    v.asDouble = params.Value.toDouble();
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 4);
                    data.setValue(0, v.asUint16[0]);
                    data.setValue(1, v.asUint16[1]);
                    data.setValue(2, v.asUint16[2]);
                    data.setValue(3, v.asUint16[3]);
                }
                break;

                case DataDisplayMode::SwappedDbl:
                {
                    union {
                       quint16 asUint16[4];
                       double asDouble;
                    } v;
                    v.asDouble = params.Value.toDouble();
                    data = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, params.Address - 1, 4);
                    data.setValue(0, v.asUint16[3]);
                    data.setValue(1, v.asUint16[2]);
                    data.setValue(2, v.asUint16[1]);
                    data.setValue(3, v.asUint16[0]);
                }
                break;
            }
        break;

        default:
        break;
    }

    if(_modbusClient == nullptr ||
       _modbusClient->state() != QModbusDevice::ConnectedState)
    {
        QMessageBox::warning(this, windowTitle(), errorDesc);
        return;
    }

    if(auto reply = _modbusClient->sendWriteRequest(data, params.Node))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, errorDesc, reply]()
            {
                if (reply->error() == QModbusDevice::ProtocolError)
                {
                    const QString exception = ModbusException(reply->rawResult().exceptionCode());
                    const QString error = QString("%1. %2").arg(errorDesc, exception);
                    QMessageBox::warning(this, windowTitle(), error);
                }
                else if (reply->error() != QModbusDevice::NoError)
                {
                    QMessageBox::warning(this, windowTitle(), QString("%1. %2").arg(errorDesc, reply->errorString()));
                }
                reply->deleteLater();
            });
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

    const auto request = createReadRequest();
    if(!request.isValid()) return;

    // update data
    ui->outputWidget->update(request);
    ui->statisticWidget->increaseNumberOfPolls();

    sendReadRequest(request, dd.DeviceId);
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
            DialogWriteCoilRegister dlg({ node, addr, value }, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                writeRegister(pointType, dlg.writeParams());
            }
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            DialogWriteHoldingRegister dlg({ node, addr, value }, dataDisplayMode(), this);
            if(dlg.exec() == QDialog::Accepted)
            {
                writeRegister(pointType, dlg.writeParams());
            }
        }
        break;

        default:
        break;
    }
}
