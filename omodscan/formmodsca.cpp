#include <QPainter>
#include <QPalette>
#include <QDateTime>
#include "modbuslimits.h"
#include "modbusexception.h"
#include "mainwindow.h"
#include "dialogwritecoilregister.h"
#include "dialogwriteholdingregister.h"
#include "dialogwriteholdingregisterbits.h"
#include "formmodsca.h"
#include "ui_formmodsca.h"

QVersionNumber FormModSca::VERSION = QVersionNumber(1, 1);

///
/// \brief FormModSca::FormModSca
/// \param id
/// \param client
/// \param ver
/// \param parent
///
FormModSca::FormModSca(int id, ModbusClient& client, MainWindow* parent)
    : QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_formId(id)
    ,_validSlaveResponses(0)
    ,_noSlaveResponsesCounter(0)
    ,_modbusClient(client)
    ,_dataSimulator(new DataSimulator(this))
{
    Q_ASSERT(parent != nullptr);

    ui->setupUi(this);
    setWindowTitle(QString("ModSca%1").arg(_formId));

    _timer.setInterval(1000);

    ui->lineEditAddress->setPaddingZeroes(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditAddress->setValue(1);

    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditLength->setValue(50);

    ui->lineEditDeviceId->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditDeviceId->setValue(1);

    ui->outputWidget->setup(displayDefinition());
    ui->outputWidget->setFocus();

    connect(_dataSimulator.get(), &DataSimulator::dataSimulated, this, &FormModSca::on_dataSimulated);
    connect(&_modbusClient, &ModbusClient::modbusDisconnected, this, &FormModSca::on_modbusDisconnected);
    connect(&_modbusClient, &ModbusClient::modbusConnected, this, &FormModSca::on_modbusConnected);
    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &FormModSca::on_modbusRequest);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &FormModSca::on_modbusReply);
    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);

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
/// \brief FormModSca::changeEvent
/// \param event
///
void FormModSca::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

///
/// \brief FormModSca::filename
/// \return
///
QString FormModSca::filename() const
{
    return _filename;
}

///
/// \brief FormModSca::setFilename
/// \param filename
///
void FormModSca::setFilename(const QString& filename)
{
    _filename = filename;
}

///
/// \brief FormModSca::data
/// \return
///
QVector<quint16> FormModSca::data() const
{
    return ui->outputWidget->data();
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

    ui->outputWidget->setStatus(tr("Data Uninitialized"));
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
/// \brief FormModSca::displayHexAddresses
/// \return
///
bool FormModSca::displayHexAddresses() const
{
    return ui->outputWidget->displayHexAddresses();
}

///
/// \brief FormModSca::setDisplayHexAddresses
/// \param on
///
void FormModSca::setDisplayHexAddresses(bool on)
{
    ui->outputWidget->setDisplayHexAddresses(on);
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
/// \brief FormModSca::byteOrder
/// \return
///
ByteOrder FormModSca::byteOrder() const
{
    return ui->outputWidget->byteOrder();
}

///
/// \brief FormModSca::setByteOrder
/// \param order
///
void FormModSca::setByteOrder(ByteOrder order)
{
    ui->outputWidget->setByteOrder(order);
    emit byteOrderChanged(order);
}

///
/// \brief FormModSca::captureMode
///
CaptureMode FormModSca::captureMode() const
{
    return ui->outputWidget->captureMode();
}

///
/// \brief FormModSca::startTextCapture
/// \param file
///
void FormModSca::startTextCapture(const QString& file)
{
    ui->outputWidget->startTextCapture(file);
}

///
/// \brief FormModSca::stopTextCapture
///
void FormModSca::stopTextCapture()
{
   ui->outputWidget->stopTextCapture();
}

///
/// \brief FormModSca::backgroundColor
/// \return
///
QColor FormModSca::backgroundColor() const
{
    return ui->outputWidget->backgroundColor();
}

///
/// \brief FormModSca::setBackgroundColor
/// \param clr
///
void FormModSca::setBackgroundColor(const QColor& clr)
{
    ui->outputWidget->setBackgroundColor(clr);
}

///
/// \brief FormModSca::foregroundColor
/// \return
///
QColor FormModSca::foregroundColor() const
{
    return ui->outputWidget->foregroundColor();
}

///
/// \brief FormModSca::setForegroundColor
/// \param clr
///
void FormModSca::setForegroundColor(const QColor& clr)
{
    ui->outputWidget->setForegroundColor(clr);
}

///
/// \brief FormModSca::statusColor
/// \return
///
QColor FormModSca::statusColor() const
{
    return ui->outputWidget->statusColor();
}

///
/// \brief FormModSca::setStatusColor
/// \param clr
///
void FormModSca::setStatusColor(const QColor& clr)
{
    ui->outputWidget->setStatusColor(clr);
}

///
/// \brief FormModSca::font
/// \return
///
QFont FormModSca::font() const
{
   return ui->outputWidget->font();
}

///
/// \brief FormModSca::setFont
/// \param font
///
void FormModSca::setFont(const QFont& font)
{
    ui->outputWidget->setFont(font);
}

///
/// \brief FormModSca::print
/// \param printer
///
void FormModSca::print(QPrinter* printer)
{
    if(!printer) return;

    auto layout = printer->pageLayout();
    const auto resolution = printer->resolution();
    auto pageRect = layout.paintRectPixels(resolution);

    const auto margin = qMax(pageRect.width(), pageRect.height()) * 0.05;
    layout.setMargins(QMargins(margin, margin, margin, margin));
    pageRect.adjust(layout.margins().left(), layout.margins().top(), -layout.margins().right(), -layout.margins().bottom());

    const int pageWidth = pageRect.width();
    const int pageHeight = pageRect.height();

    const int cx = pageRect.left();
    const int cy = pageRect.top();

    QPainter painter(printer);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const auto textTime = QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat);
    auto rcTime = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextSingleLine, textTime);

    const auto textDevId = QString(tr("Device Id: %1")).arg(ui->lineEditDeviceId->text());
    auto rcDevId = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextSingleLine, textDevId);

    const auto textAddrLen = QString(tr("Address: %1\nLength: %2")).arg(ui->lineEditAddress->text(), ui->lineEditLength->text());
    auto rcAddrLen = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textAddrLen);

    const auto textType = QString(tr("MODBUS Point Type:\n%1")).arg(ui->comboBoxModbusPointType->currentText());
    auto rcType = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textType);

    const auto textStat = QString(tr("Number of Polls: %1\nValid Slave Responses: %2")).arg(QString::number(ui->statisticWidget->numberOfPolls()),
                                                                                        QString::number(ui->statisticWidget->validSlaveResposes()));
    auto rcStat = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textStat);

    rcTime.moveTopRight({ pageRect.right(), 10 });
    rcDevId.moveLeft(rcAddrLen.right() + 40);
    rcAddrLen.moveTop(rcDevId.bottom() + 10);
    rcType.moveTopLeft({ rcDevId.left(), rcAddrLen.top() });
    rcStat.moveLeft(rcType.right() + 40);

    painter.drawText(rcTime, Qt::TextSingleLine, textTime);
    painter.drawText(rcDevId, Qt::TextSingleLine, textDevId);
    painter.drawText(rcAddrLen, Qt::TextWordWrap, textAddrLen);
    painter.drawText(rcType, Qt::TextWordWrap, textType);
    painter.drawText(rcStat, Qt::TextWordWrap, textStat);
    painter.drawRect(rcStat.adjusted(-2, -2, 40, 2));

    auto rcOutput = pageRect;
    rcOutput.setTop(rcAddrLen.bottom() + 20);

    ui->outputWidget->paint(rcOutput, painter);
}

///
/// \brief FormModSca::resetCtrls
///
void FormModSca::resetCtrs()
{
    ui->statisticWidget->resetCtrs();
}

///
/// \brief FormModSca::numberOfPolls
/// \return
///
uint FormModSca::numberOfPolls() const
{
    return ui->statisticWidget->numberOfPolls();
}

///
/// \brief FormModSca::validSlaveResposes
/// \return
///
uint FormModSca::validSlaveResposes() const
{
    return ui->statisticWidget->validSlaveResposes();
}

///
/// \brief FormModSca::show
///
void FormModSca::show()
{
    QWidget::show();
    emit showed();
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{
    if(!_modbusClient.isValid()) return;
    if(_modbusClient.state() != QModbusDevice::ConnectedState)
    {
        ui->outputWidget->setStatus(tr("Device NOT CONNECTED!"));
        return;
    }

    const auto dd = displayDefinition();
    if(dd.PointAddress + dd.Length - 1 > ModbusLimits::addressRange().to())
    {
        ui->outputWidget->setStatus(tr("Invalid Data Length Specified"));
        return;
    }

    if(_validSlaveResponses == ui->statisticWidget->validSlaveResposes())
    {
        _noSlaveResponsesCounter++;
        if(_noSlaveResponsesCounter > _modbusClient.numberOfRetries())
        {
            ui->outputWidget->setStatus(tr("No Responses from Slave Device"));
        }
    }

    _modbusClient.sendReadRequest(dd.PointType, dd.PointAddress - 1, dd.Length, dd.DeviceId, _formId);
}

///
/// \brief FormModSca::isValidReply
/// \param reply
/// \return
///
bool FormModSca::isValidReply(const QModbusReply* reply)
{
    const auto dd = displayDefinition();
    const auto data = reply->result();
    const auto response = reply->rawResult();

    switch(response.functionCode())
    {
        case QModbusRequest::ReadCoils:
        case QModbusRequest::ReadDiscreteInputs:
            return (data.startAddress() == dd.PointAddress - 1) && (data.valueCount() - dd.Length) < 8;
        break;

        case QModbusRequest::ReadInputRegisters:
        case QModbusRequest::ReadHoldingRegisters:
            return (data.valueCount() == dd.Length) && (data.startAddress() == dd.PointAddress - 1);

        default:
            break;
    }

    return false;
}

///
/// \brief FormModSca::on_modbusReply
/// \param reply
///
void FormModSca::on_modbusReply(QModbusReply* reply)
{
    if(!reply) return;

    if(_formId != reply->property("RequestId").toInt())
    {
        return;
    }

    const auto data = reply->result();
    const auto response = reply->rawResult();

    switch(response.functionCode())
    {
        case QModbusRequest::ReadCoils:
        case QModbusRequest::ReadDiscreteInputs:
        case QModbusRequest::ReadInputRegisters:
        case QModbusRequest::ReadHoldingRegisters:
        break;

        default:
        return;
    }

    ui->outputWidget->updateTraffic(response, reply->serverAddress());

    if (reply->error() == QModbusDevice::NoError)
    {
        if(!isValidReply(reply))
        {
            ui->outputWidget->setStatus(tr("Received Invalid Response MODBUS Query"));
        }
        else
        {
            ui->outputWidget->updateData(data);
            ui->outputWidget->setStatus(QString());
            ui->statisticWidget->increaseValidSlaveResponses();
        }
    }
    else if (reply->error() == QModbusDevice::ProtocolError)
    {
        ui->outputWidget->setStatus(ModbusException(response.exceptionCode()));
    }
    else
    {
        ui->outputWidget->setStatus(reply->errorString());
    }

    _noSlaveResponsesCounter = 0;
    _validSlaveResponses = ui->statisticWidget->validSlaveResposes();
}

///
/// \brief FormModSca::on_modbusRequest
/// \param requestId
/// \param request
///
void FormModSca::on_modbusRequest(int requestId, const QModbusRequest& request)
{
    if(requestId != _formId)
    {
        return;
    }

    switch(request.functionCode())
    {
        case QModbusPdu::ReadCoils:
        case QModbusPdu::ReadDiscreteInputs:
        case QModbusPdu::ReadHoldingRegisters:
        case QModbusPdu::ReadInputRegisters:
        {
            const auto deviceId = ui->lineEditDeviceId->value<int>();
            ui->outputWidget->updateTraffic(request, deviceId);
            ui->statisticWidget->increaseNumberOfPolls();
        }
        break;

    default:
        break;
    }
}

///
/// \brief FormModSca::on_modbusConnected
///
void FormModSca::on_modbusConnected(const ConnectionDetails&)
{
    _dataSimulator->resumeSimulations();
}

///
/// \brief FormModSca::on_modbusDisconnected
///
void FormModSca::on_modbusDisconnected(const ConnectionDetails&)
{
    _dataSimulator->pauseSimulations();
}

///
/// \brief FormModSca::on_dataSimulated
/// \param mode
/// \param type
/// \param addr
/// \param value
///
void FormModSca::on_dataSimulated(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, QVariant value)
{
    if(!_modbusClient.isValid() ||
        _modbusClient.state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    const quint32 node = ui->lineEditDeviceId->value<int>();
    ModbusWriteParams params = { node, addr, value, mode, byteOrder() };

    _modbusClient.writeRegister(type, params, _formId);
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
void FormModSca::on_outputWidget_itemDoubleClicked(quint16 addr, const QVariant& value)
{
    if(!_modbusClient.isValid() ||
        _modbusClient.state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    const auto mode = dataDisplayMode();
    const quint32 node = ui->lineEditDeviceId->value<int>();
    const auto pointType = ui->comboBoxModbusPointType->currentPointType();
    auto&& simParams = _simulationMap[{pointType, addr}];

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        {
            ModbusWriteParams params = { node, addr, value, mode, byteOrder() };
            DialogWriteCoilRegister dlg(params, simParams, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                if(simParams.Mode == SimulationMode::No) _modbusClient.writeRegister(pointType, params, _formId);
                _dataSimulator->startSimulation(mode, pointType, addr, simParams);
            }
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            ModbusWriteParams params = { node, addr, value, mode, byteOrder()};
            if(mode == DataDisplayMode::Binary)
            {
                DialogWriteHoldingRegisterBits dlg(params, this);
                if(dlg.exec() == QDialog::Accepted)
                    _modbusClient.writeRegister(pointType, params, _formId);
            }
            else
            {
                DialogWriteHoldingRegister dlg(params, simParams, mode, this);
                if(dlg.exec() == QDialog::Accepted)
                {
                    if(simParams.Mode == SimulationMode::No) _modbusClient.writeRegister(pointType, params, _formId);
                    _dataSimulator->startSimulation(mode, pointType, addr, simParams);
                }
            }
        }
        break;

        default:
        break;
    }
}

///
/// \brief FormModSca::on_statisticWidget_numberOfPollsChanged
/// \param value
///
void FormModSca::on_statisticWidget_numberOfPollsChanged(uint value)
{
    emit numberOfPollsChanged(value);
}

///
/// \brief FormModSca::on_statisticWidget_validSlaveResposesChanged
/// \param value
///
void FormModSca::on_statisticWidget_validSlaveResposesChanged(uint value)
{
    emit validSlaveResposesChanged(value);
}
