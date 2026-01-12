#include <QMouseEvent>
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

QVersionNumber FormModSca::VERSION = QVersionNumber(1, 8);

///
/// \brief FormModSca::FormModSca
/// \param id
/// \param client
/// \param ver
/// \param parent
///
FormModSca::FormModSca(int id, ModbusClient& client, DataSimulator* simulator, MainWindow* parent)
    : QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_formId(id)
    ,_validSlaveResponses(0)
    ,_noSlaveResponsesCounter(0)
    ,_modbusClient(client)
    ,_dataSimulator(simulator)
    ,_parent(parent)
{
    Q_ASSERT(parent != nullptr);
    Q_ASSERT(simulator != nullptr);

    ui->setupUi(this);
    setWindowTitle(QString("ModSca%1").arg(_formId));

    _timer.setInterval(1000);

    ui->lineEditAddress->blockSignals(true);
    ui->lineEditAddress->setLeadingZeroes(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(true));
    ui->lineEditAddress->setValue(1);
    ui->lineEditAddress->blockSignals(false);

    ui->lineEditLength->blockSignals(true);
    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditLength->setValue(50);
    ui->lineEditLength->blockSignals(false);

    ui->lineEditDeviceId->blockSignals(true);
    ui->lineEditDeviceId->setLeadingZeroes(true);
    ui->lineEditDeviceId->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditDeviceId->setValue(1);
    ui->lineEditDeviceId->blockSignals(false);

    ui->comboBoxAddressBase->blockSignals(true);
    ui->comboBoxAddressBase->setCurrentAddressBase(AddressBase::Base1);
    ui->comboBoxAddressBase->blockSignals(false);

    const auto dd = displayDefinition();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(dd, protocol, _dataSimulator->simulationMap(dd.DeviceId));
    ui->outputWidget->setFocus();
    connect(ui->outputWidget, &OutputWidget::startTextCaptureError, this, &FormModSca::captureError);

    setPollState(PollState::Off);
    connect(ui->statisticWidget, &StatisticWidget::ctrsReseted, ui->outputWidget, &OutputWidget::clearLogView);
    connect(ui->statisticWidget, &StatisticWidget::pollStateChanged, this, [&](PollState state) {
        switch (state) {
        case PollState::Off: break;
        case PollState::Paused:
            ui->outputWidget->setStatus(tr("Device polling paused..."), _modbusClient.state());
            _timer.stop();
        break;
        case PollState::Running:
            ui->outputWidget->setStatus("", _modbusClient.state());
            beginUpdate();
            _timer.start();
        break;
        }
    });

    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &FormModSca::on_modbusRequest);
    connect(&_modbusClient, &ModbusClient::modbusResponse, this, &FormModSca::on_modbusResponse);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &FormModSca::on_modbusReply);
    connect(&_modbusClient, &ModbusClient::modbusConnected, this, &FormModSca::on_modbusConnected);
    connect(&_modbusClient, &ModbusClient::modbusDisconnected, this, &FormModSca::on_modbusDisconnected);
    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);

    connect(_dataSimulator, &DataSimulator::simulationStarted, this, &FormModSca::on_simulationStarted);
    connect(_dataSimulator, &DataSimulator::simulationStopped, this, &FormModSca::on_simulationStopped);
    connect(_dataSimulator, &DataSimulator::dataSimulated, this, &FormModSca::on_dataSimulated);

    ui->frameDataDefinition->installEventFilter(this);
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
/// \brief FormModSca::eventFilter
/// \param watched
/// \param event
/// \return
///
bool FormModSca::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->frameDataDefinition && event->type() == QEvent::MouseButtonDblClick) {
        auto* me = static_cast<QMouseEvent*>(event);
        if(me->pos().x() > ui->statisticWidget->geometry().right()) {
            emit doubleClicked();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
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
    dd.FormName = windowTitle();
    dd.ScanRate = _timer.interval();
    dd.DeviceId = ui->lineEditDeviceId->value<int>();
    dd.PointAddress = ui->lineEditAddress->value<int>();
    dd.PointType = ui->comboBoxModbusPointType->currentPointType();
    dd.Length = ui->lineEditLength->value<int>();
    dd.LogViewLimit = ui->outputWidget->logViewLimit();
    dd.AutoscrollLog = ui->outputWidget->autoscrollLogView();
    dd.ZeroBasedAddress = ui->comboBoxAddressBase->currentAddressBase() == AddressBase::Base0;
    dd.HexAddress = displayHexAddresses();
    dd.DataViewColumnsDistance = ui->outputWidget->dataViewColumnsDistance();
    dd.LeadingZeros = ui->lineEditDeviceId->leadingZeroes();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    if(!dd.FormName.isEmpty())
        setWindowTitle(dd.FormName);

    _timer.setInterval(dd.ScanRate);

    ui->lineEditDeviceId->blockSignals(true);
    ui->lineEditDeviceId->setLeadingZeroes(dd.LeadingZeros);
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditDeviceId->blockSignals(false);

    ui->comboBoxAddressBase->blockSignals(true);
    ui->comboBoxAddressBase->setCurrentAddressBase(dd.ZeroBasedAddress ? AddressBase::Base0 : AddressBase::Base1);
    ui->comboBoxAddressBase->blockSignals(false);

    ui->lineEditAddress->blockSignals(true);
    ui->lineEditAddress->setLeadingZeroes(dd.LeadingZeros);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(dd.ZeroBasedAddress));
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditAddress->blockSignals(false);

    ui->lineEditLength->blockSignals(true);
    ui->lineEditLength->setValue(dd.Length);
    ui->lineEditLength->blockSignals(false);

    ui->comboBoxModbusPointType->blockSignals(true);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);
    ui->comboBoxModbusPointType->blockSignals(false);

    ui->outputWidget->setStatus(tr("Data Uninitialized"), _modbusClient.state());

    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(dd, protocol, _dataSimulator->simulationMap(dd.DeviceId));

    setDisplayHexAddresses(dd.HexAddress);

    beginUpdate();
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

    ui->lineEditAddress->setInputMode(on ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(ui->comboBoxAddressBase->currentAddressBase() == AddressBase::Base0));
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
/// \brief FormModSca::codepage
/// \return
///
QString FormModSca::codepage() const
{
    return ui->outputWidget->codepage();
}

///
/// \brief FormModSca::setCodepage
/// \param name
///
void FormModSca::setCodepage(const QString& name)
{
    ui->outputWidget->setCodepage(name);
    emit codepageChanged(name);
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

    const auto textAddrLen = QString(tr("Address Base: %1\nStart Address: %2\nLength: %3")).arg(ui->comboBoxAddressBase->currentText(),
                                                                                                ui->lineEditAddress->text(), ui->lineEditLength->text());
    auto rcAddrLen = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textAddrLen);

    const auto textDevIdType = QString(tr("Device Id: %1\nMODBUS Point Type:\n%2")).arg(ui->lineEditDeviceId->text(), ui->comboBoxModbusPointType->currentText());
    auto rcDevIdType = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textDevIdType);

    const auto textStat = QString(tr("Number of Polls: %1\nValid Slave Responses: %2")).arg(QString::number(ui->statisticWidget->numberOfPolls()),
                                                                                        QString::number(ui->statisticWidget->validSlaveResposes()));
    auto rcStat = painter.boundingRect(cx, cy, pageWidth, pageHeight, Qt::TextWordWrap, textStat);

    rcTime.moveTopRight({ pageRect.right(), 10 });
    rcDevIdType.moveTopLeft({ rcAddrLen.right() + 40, rcAddrLen.top()});
    rcStat.moveLeft(rcDevIdType.right() + 40);

    painter.drawText(rcTime, Qt::TextSingleLine, textTime);
    painter.drawText(rcAddrLen, Qt::TextWordWrap, textAddrLen);
    painter.drawText(rcDevIdType, Qt::TextWordWrap, textDevIdType);
    painter.drawText(rcStat, Qt::TextWordWrap, textStat);
    painter.drawRect(rcStat.adjusted(-2, -2, 40, 2));

    auto rcOutput = pageRect;
    rcOutput.setTop(rcAddrLen.bottom() + 20);

    ui->outputWidget->paint(rcOutput, painter);
}

///
/// \brief FormModSca::simulationMap
/// \return
///
ModbusSimulationMap FormModSca::simulationMap() const
{
    const auto dd = displayDefinition();
    const auto startAddr = dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1);
    const auto endAddr = startAddr + dd.Length;

    ModbusSimulationMap result;
    const auto simulationMap = _dataSimulator->simulationMap(dd.DeviceId);
    for(auto&& key : simulationMap.keys())
    {
        if(key.first == dd.PointType &&
           key.second >= startAddr && key.second < endAddr)
        {
            result[key] = simulationMap[key];
        }
    }

    return result;
}

///
/// \brief FormModSca::startSimulation
/// \param type
/// \param addr
/// \param params
///
void FormModSca::startSimulation(QModbusDataUnit::RegisterType type, quint16 addr, const ModbusSimulationParams& params)
{
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    _dataSimulator->startSimulation(dataDisplayMode(), type, addr, deviceId, params);
    if(_modbusClient.state() != ModbusDevice::ConnectedState) _dataSimulator->pauseSimulations();
}

///
/// \brief FormModSca::descriptionMap
/// \return
///
AddressDescriptionMap FormModSca::descriptionMap() const
{
    return ui->outputWidget->descriptionMap();
}

///
/// \brief FormModSca::setDescription
/// \param type
/// \param addr
/// \param desc
///
void FormModSca::setDescription(QModbusDataUnit::RegisterType type, quint16 addr, const QString& desc)
{
    ui->outputWidget->setDescription(type, addr, desc);
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
/// \brief FormModSca::pollState
/// \return
///
PollState FormModSca::pollState() const
{
    return ui->statisticWidget->pollState();
}

///
/// \brief FormModSca::setPollState
/// \param state
///
void FormModSca::setPollState(PollState state)
{
    ui->statisticWidget->setPollState(state);
}

///
/// \brief FormModSca::show
///
void FormModSca::show()
{
    QWidget::show();
    emit showed();

    beginUpdate();
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{
    if(_modbusClient.state() != ModbusDevice::ConnectedState)
        return;

    const auto dd = displayDefinition();
    const auto addr = dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1);
    if(addr + dd.Length <= ModbusLimits::addressRange(dd.ZeroBasedAddress).to())
    {
        if(_validSlaveResponses == ui->statisticWidget->validSlaveResposes())
        {
            if(_noSlaveResponsesCounter > _modbusClient.numberOfRetries())
            {
                ui->outputWidget->setStatus(tr("No Responses from Slave Device"), _modbusClient.state());
            }
            _noSlaveResponsesCounter++;
        }

        _modbusClient.sendReadRequest(dd.PointType, addr, dd.Length, dd.DeviceId, _formId);
    }
}

///
/// \brief FormModSca::beginUpdate
///
void FormModSca::beginUpdate()
{
    if(_modbusClient.state() != ModbusDevice::ConnectedState) {
        setPollState(PollState::Off);
        return;
    }

    const auto dd = displayDefinition();
    const auto addr = dd.PointAddress - (dd.ZeroBasedAddress ?  0 : 1);
    if(addr + dd.Length <= ModbusLimits::addressRange(dd.ZeroBasedAddress).to()) {
        _modbusClient.sendReadRequest(dd.PointType, addr, dd.Length, dd.DeviceId, _formId);
    }
    else
        ui->outputWidget->setStatus(tr("No Scan: Invalid Data Length Specified"), _modbusClient.state());

    if(pollState() == PollState::Off) {
        setPollState(PollState::Running);
    }
}

///
/// \brief FormModSca::isValidReply
/// \param reply
/// \return
///
bool FormModSca::isValidReply(const ModbusReply* const reply) const
{
    const auto dd = displayDefinition();
    if(reply->serverAddress() != dd.DeviceId) {
        return false;
    }

    const auto data = reply->result();
    const auto response = reply->rawResult();
    const auto addr = dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1);

    switch(response.functionCode())
    {
        case QModbusPdu::ReadCoils:
        case QModbusPdu::ReadDiscreteInputs:
            return (data.startAddress() == addr) && (data.valueCount() - dd.Length) < 8;
        break;

        case QModbusPdu::ReadInputRegisters:
        case QModbusPdu::ReadHoldingRegisters:
            return (data.valueCount() == dd.Length) && (data.startAddress() == addr);

        default:
            return true;
    }
}

///
/// \brief FormModSca::logModbusMessage
/// \param requestGroupId
/// \param msg
///
void FormModSca::logModbusMessage(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(!msg)
        return;

    if(requestGroupId == _formId)
        ui->outputWidget->updateTraffic(msg);
    else if(requestGroupId == 0 && isActive())
        ui->outputWidget->updateTraffic(msg);
}

///
/// \brief FormModSca::on_modbusRequest
/// \param requestId
/// \param msg
///
void FormModSca::on_modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(!msg)
        return;

    logModbusMessage(requestGroupId, msg);

    switch(msg->functionCode())
    {
        case QModbusPdu::ReadCoils:
        case QModbusPdu::ReadDiscreteInputs:
        case QModbusPdu::ReadHoldingRegisters:
        case QModbusPdu::ReadInputRegisters:
            if(requestGroupId == _formId)
                ui->statisticWidget->increaseNumberOfPolls();
        break;

        default:
        break;
    }
}

///
/// \brief FormModSca::on_modbusResponse
/// \param requestGroupId
/// \param msg
///
void FormModSca::on_modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(!msg)
        return;

    logModbusMessage(requestGroupId, msg);
}

///
/// \brief FormModSca::on_modbusReply
/// \param reply
///
void FormModSca::on_modbusReply(const ModbusReply* const reply)
{
    if(!reply) return;

    const QModbusResponse response = reply->rawResult();
    const bool hasError = reply->error() != ModbusDevice::NoError;

    switch(response.functionCode())
    {
        case QModbusRequest::ReadCoils:
        case QModbusRequest::ReadDiscreteInputs:
        case QModbusRequest::ReadInputRegisters:
        case QModbusRequest::ReadHoldingRegisters:
        break;

        default:
            if(!hasError) beginUpdate();
        return;
    }

    if(reply->requestGroupId() != _formId) {
        return;
    }

    if (!hasError)
    {
        if(!isValidReply(reply))
        {
            ui->outputWidget->setStatus(tr("Received Invalid Response MODBUS Query"), _modbusClient.state());
        }
        else
        {
            ui->outputWidget->updateData(reply->result());

            if(pollState() != PollState::Paused) {
                ui->outputWidget->setStatus(QString(), _modbusClient.state());
            }
            ui->statisticWidget->increaseValidSlaveResponses();
        }
    }
    else if (reply->error() == ModbusDevice::ProtocolError)
    {
        const auto ex = ModbusException(response.exceptionCode());
        const auto errorString = QString("%1 (%2)").arg(ex, formatUInt8Value(DataDisplayMode::Hex, true, ex));
        ui->outputWidget->setStatus(errorString, _modbusClient.state());
    }
    else
    {

        ui->outputWidget->setStatus(reply->errorString(), _modbusClient.state());
    }

    _noSlaveResponsesCounter = 0;
    _validSlaveResponses = ui->statisticWidget->validSlaveResposes();
}

///
/// \brief FormModSca::on_modbusConnected
///
void FormModSca::on_modbusConnected(const ConnectionDetails&)
{
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setProtocol(protocol);
    ui->outputWidget->clearLogView();

    setPollState(PollState::Running);
}

///
/// \brief FormModSca::on_modbusDisconnected
///
void FormModSca::on_modbusDisconnected(const ConnectionDetails&)
{
    setPollState(PollState::Off);
    ui->outputWidget->setStatus(tr("Device NOT CONNECTED!"), _modbusClient.state());
}

///
/// \brief FormModSca::on_lineEditAddress_valueChanged
///
void FormModSca::on_lineEditAddress_valueChanged(const QVariant&)
{
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(displayDefinition(), protocol, _dataSimulator->simulationMap(deviceId));

    beginUpdate();
}

///
/// \brief FormModSca::on_lineEditLength_valueChanged
///
void FormModSca::on_lineEditLength_valueChanged(const QVariant&)
{
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(displayDefinition(), protocol, _dataSimulator->simulationMap(deviceId));

    beginUpdate();
}

///
/// \brief FormModSca::on_lineEditDeviceId_valueChanged
///
void FormModSca::on_lineEditDeviceId_valueChanged(const QVariant&)
{
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(displayDefinition(), protocol, _dataSimulator->simulationMap(deviceId));

    beginUpdate();
}

///
/// \brief FormModSca:on_comboBoxAddressBase_addressBaseChanged
/// \param index
///
void FormModSca::on_comboBoxAddressBase_addressBaseChanged(AddressBase base)
{
    auto dd = displayDefinition();
    dd.PointAddress = (base == AddressBase::Base1 ? qMax(1, dd.PointAddress + 1) : qMax(0, dd.PointAddress - 1));
    dd.ZeroBasedAddress = (base == AddressBase::Base0);
    setDisplayDefinition(dd);
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_pointTypeChanged
/// \param type
///
void FormModSca::on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType type)
{
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(displayDefinition(), protocol, _dataSimulator->simulationMap(deviceId));

    emit pointTypeChanged(type);

    beginUpdate();
}

///
/// \brief FormModSca::on_outputWidget_itemDoubleClicked
/// \param addr
/// \param value
///
void FormModSca::on_outputWidget_itemDoubleClicked(quint16 addr, const QVariant& value)
{
    if(!_modbusClient.isValid() ||
        _modbusClient.state() != ModbusDevice::ConnectedState)
    {
        return;
    }

    const auto dd = displayDefinition();
    const auto mode = dataDisplayMode();
    const auto zeroBasedAddress = dd.ZeroBasedAddress;
    const auto addressSpace = dd.AddrSpace;
    const auto simAddr = addr - (zeroBasedAddress ? 0 : 1);
    auto simParams = _dataSimulator->simulationParams(dd.PointType, simAddr, dd.DeviceId);

    switch(dd.PointType)
    {
        case QModbusDataUnit::Coils:
        {
            ModbusWriteParams params;
            params.DeviceId = dd.DeviceId;
            params.Address = addr;
            params.Value = value;
            params.DisplayMode = mode;
            params.AddrSpace = dd.AddrSpace;
            params.Order = byteOrder();
            params.Codepage = codepage();
            params.ZeroBasedAddress = dd.ZeroBasedAddress;
            params.ForceModbus15And16Func = _modbusClient.isForcedModbus15And16Func();

            DialogWriteCoilRegister dlg(params, simParams, displayHexAddresses(), _parent);
            switch(dlg.exec())
            {
                case QDialog::Accepted:
                    _modbusClient.writeRegister(dd.PointType, params, _formId);
                break;

                case 2:
                    if(simParams.Mode == SimulationMode::Off) _dataSimulator->stopSimulation(dd.PointType, simAddr, dd.DeviceId);
                    else _dataSimulator->startSimulation(mode, dd.PointType, simAddr, dd.DeviceId, simParams);
                break;
            }
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            ModbusWriteParams params;
            params.DeviceId = dd.DeviceId;
            params.Address = addr;
            params.Value = value;
            params.DisplayMode = mode;
            params.AddrSpace = dd.AddrSpace;
            params.Order = byteOrder();
            params.Codepage = codepage();
            params.ZeroBasedAddress = dd.ZeroBasedAddress;
            params.LeadingZeros = dd.LeadingZeros;
            params.ForceModbus15And16Func = _modbusClient.isForcedModbus15And16Func();

            if(mode == DataDisplayMode::Binary)
            {
                DialogWriteHoldingRegisterBits dlg(params, displayHexAddresses(), _parent);
                if(dlg.exec() == QDialog::Accepted)
                    _modbusClient.writeRegister(dd.PointType, params, _formId);
            }
            else
            {
                DialogWriteHoldingRegister dlg(params, simParams, displayHexAddresses(), _parent);
                switch(dlg.exec())
                {
                    case QDialog::Accepted:
                        _modbusClient.writeRegister(dd.PointType, params, _formId);
                    break;

                    case 2:
                        if(simParams.Mode == SimulationMode::Off) _dataSimulator->stopSimulation(dd.PointType, simAddr, dd.DeviceId);
                        else _dataSimulator->startSimulation(mode, dd.PointType, simAddr, dd.DeviceId, simParams);
                    break;
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

///
/// \brief FormModSca::on_simulationStarted
/// \param type
/// \param addr
///
void FormModSca::on_simulationStarted(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId)
{
    if(deviceId != ui->lineEditDeviceId->value<int>())
        return;

    ui->outputWidget->setSimulated(type, addr, true);
}

///
/// \brief FormModSca::on_simulationStopped
/// \param type
/// \param addr
///
void FormModSca::on_simulationStopped(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId)
{
    if(deviceId != ui->lineEditDeviceId->value<int>())
        return;

    ui->outputWidget->setSimulated(type, addr, false);
}

///
/// \brief FormModSca::on_dataSimulated
/// \param mode
/// \param type
/// \param addr
/// \param value
///
void FormModSca::on_dataSimulated(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, QVariant value)
{
    if(_modbusClient.state() != ModbusDevice::ConnectedState)
    {
        return;
    }

    const auto dd = displayDefinition();
    if(dd.DeviceId != deviceId)
    {
        return;
    }

    const auto pointAddr = dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1);
    if(type == dd.PointType && addr >= pointAddr && addr <= pointAddr + dd.Length)
    {
        const ModbusWriteParams params = { dd.DeviceId, addr, value, mode, dd.AddrSpace, byteOrder(), codepage(), true };
        _modbusClient.writeRegister(type, params, formId());
    }
}
