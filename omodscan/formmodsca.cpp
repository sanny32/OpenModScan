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

QVersionNumber FormModSca::VERSION = QVersionNumber(1, 7);

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

    ui->lineEditAddress->setPaddingZeroes(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(true));
    ui->lineEditAddress->setValue(0);

    ui->lineEditLength->setInputRange(ModbusLimits::lengthRange());
    ui->lineEditLength->setValue(50);

    ui->lineEditDeviceId->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditDeviceId->setValue(1);

    ui->comboBoxAddressBase->setCurrentAddressBase(AddressBase::Base1);

    const auto dd = displayDefinition();
    const auto protocol = _modbusClient.connectionType() == ConnectionType::Serial ? ModbusMessage::Rtu : ModbusMessage::Tcp;
    ui->outputWidget->setup(dd, protocol, _dataSimulator->simulationMap(dd.DeviceId));
    ui->outputWidget->setFocus();

    setPollState(Off);
    connect(ui->statisticWidget, &StatisticWidget::ctrsReseted, ui->outputWidget, &OutputWidget::clearLogView);
    connect(ui->statisticWidget, &StatisticWidget::pollStateChanged, this, [&](PollState state) {
        switch (state) {
        case Off: break;
        case Paused:
            ui->outputWidget->setStatus(tr("Device polling paused..."));
            _timer.stop();
        break;
        case Running:
            ui->outputWidget->setStatus("");
            beginUpdate();
            _timer.start();
        break;
        }
    });

    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &FormModSca::on_modbusRequest);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &FormModSca::on_modbusReply);
    connect(&_modbusClient, &ModbusClient::modbusConnected, this, &FormModSca::on_modbusConnected);
    connect(&_modbusClient, &ModbusClient::modbusDisconnected, this, &FormModSca::on_modbusDisconnected);
    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);

    connect(_dataSimulator, &DataSimulator::simulationStarted, this, &FormModSca::on_simulationStarted);
    connect(_dataSimulator, &DataSimulator::simulationStopped, this, &FormModSca::on_simulationStopped);
    connect(_dataSimulator, &DataSimulator::dataSimulated, this, &FormModSca::on_dataSimulated);
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
    dd.LogViewLimit = ui->outputWidget->logViewLimit();
    dd.AutoscrollLog = ui->outputWidget->autoscrollLogView();
    dd.ZeroBasedAddress = ui->comboBoxAddressBase->currentAddressBase() == AddressBase::Base0;
    dd.HexAddress = displayHexAddresses();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    _timer.setInterval(dd.ScanRate);

    ui->lineEditDeviceId->blockSignals(true);
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditDeviceId->blockSignals(false);

    ui->comboBoxAddressBase->blockSignals(true);
    ui->comboBoxAddressBase->setCurrentAddressBase(dd.ZeroBasedAddress ? AddressBase::Base0 : AddressBase::Base1);
    ui->comboBoxAddressBase->blockSignals(false);

    ui->lineEditAddress->blockSignals(true);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(dd.ZeroBasedAddress));
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditAddress->blockSignals(false);

    ui->lineEditLength->blockSignals(true);
    ui->lineEditLength->setValue(dd.Length);
    ui->lineEditLength->blockSignals(false);

    ui->comboBoxModbusPointType->blockSignals(true);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);
    ui->comboBoxModbusPointType->blockSignals(false);

    ui->outputWidget->setStatus(tr("Data Uninitialized"));

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
    if(_modbusClient.state() != QModbusDevice::ConnectedState) _dataSimulator->pauseSimulations();
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
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{
    if(_modbusClient.state() != QModbusDevice::ConnectedState)
        return;

    const auto dd = displayDefinition();
    const auto addr = dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1);
    if(addr + dd.Length <= ModbusLimits::addressRange(dd.ZeroBasedAddress).to())
    {
        if(_validSlaveResponses == ui->statisticWidget->validSlaveResposes())
        {
            _noSlaveResponsesCounter++;
            if(_noSlaveResponsesCounter > _modbusClient.numberOfRetries())
            {
                ui->outputWidget->setStatus(tr("No Responses from Slave Device"));
            }
        }

        _modbusClient.sendReadRequest(dd.PointType, addr, dd.Length, dd.DeviceId, _formId);
    }
}

///
/// \brief FormModSca::beginUpdate
///
void FormModSca::beginUpdate()
{
    if(_modbusClient.state() != QModbusDevice::ConnectedState) {
        setPollState(Off);
        return;
    }

    const auto dd = displayDefinition();
    const auto addr = dd.PointAddress - (dd.ZeroBasedAddress ?  0 : 1);
    if(addr + dd.Length <= ModbusLimits::addressRange(dd.ZeroBasedAddress).to())
        _modbusClient.sendReadRequest(dd.PointType, addr, dd.Length, dd.DeviceId, _formId);
    else
        ui->outputWidget->setStatus(tr("No Scan: Invalid Data Length Specified"));

    if(pollState() == Off) {
        _timer.start();
        setPollState(Running);
    }
}

///
/// \brief FormModSca::isValidReply
/// \param reply
/// \return
///
bool FormModSca::isValidReply(const QModbusReply* reply) const
{
    const auto dd = displayDefinition();
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
/// \brief FormModSca::logRequest
/// \param requestId
/// \param deviceId
/// \param transactionId
/// \param request
///
void FormModSca::logRequest(int requestId, int deviceId, int transactionId, const QModbusRequest& request)
{
    if(requestId == _formId && deviceId == ui->lineEditDeviceId->value<int>())
        ui->outputWidget->updateTraffic(request, deviceId, transactionId);
    else if(requestId == 0 && isActive())
        ui->outputWidget->updateTraffic(request, deviceId, transactionId);
}

///
/// \brief FormModSca::on_modbusRequest
/// \param requestId
/// \param deviceId
/// \param transactionId
/// \param request
///
void FormModSca::on_modbusRequest(int requestId, int deviceId, int transactionId, const QModbusRequest& request)
{
   logRequest(requestId, deviceId, transactionId, request);

    switch(request.functionCode())
    {
        case QModbusPdu::ReadCoils:
        case QModbusPdu::ReadDiscreteInputs:
        case QModbusPdu::ReadHoldingRegisters:
        case QModbusPdu::ReadInputRegisters:
            if(requestId == _formId)
                ui->statisticWidget->increaseNumberOfPolls();
        break;

        default:
        break;
    }
}

///
/// \brief FormModSca::logReply
/// \param reply
///
void FormModSca::logReply(const QModbusReply* reply)
{
    if(!reply) return;

    if(reply->error() != QModbusDevice::NoError &&
        reply->error() != QModbusDevice::ProtocolError)
    {
        return;
    }

    const auto deviceId = reply->serverAddress();
    const auto requestId = reply->property("RequestId").toInt();
    const auto transactionId = reply->property("TransactionId").toInt();

    if(requestId == _formId && deviceId == ui->lineEditDeviceId->value<int>())
        ui->outputWidget->updateTraffic(reply->rawResult(), reply->serverAddress(), transactionId);
    else if(requestId == 0 && isActive())
        ui->outputWidget->updateTraffic(reply->rawResult(), reply->serverAddress(), transactionId);
}

///
/// \brief FormModSca::on_modbusReply
/// \param reply
///
void FormModSca::on_modbusReply(QModbusReply* reply)
{
    if(!reply) return;

    logReply(reply);

    const auto response = reply->rawResult();
    const bool hasError = reply->error() != QModbusDevice::NoError;

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

    if(reply->property("RequestId").toInt() != _formId)
    return;

    if (!hasError)
    {
        if(!isValidReply(reply))
        {
            ui->outputWidget->setStatus(tr("Received Invalid Response MODBUS Query"));
        }
        else
        {
            ui->outputWidget->updateData(reply->result());
            ui->outputWidget->setStatus(QString());
            ui->statisticWidget->increaseValidSlaveResponses();
        }
    }
    else if (reply->error() == QModbusDevice::ProtocolError)
    {
        const auto ex = ModbusException(response.exceptionCode());
        const auto errorString = QString("%1 (%2)").arg(ex, formatUInt8Value(DataDisplayMode::Hex, ex));
        ui->outputWidget->setStatus(errorString);
    }
    else
    {
        ui->outputWidget->setStatus(reply->errorString());
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

    beginUpdate();
}

///
/// \brief FormModSca::on_modbusDisconnected
///
void FormModSca::on_modbusDisconnected(const ConnectionDetails&)
{
    _timer.stop();
    setPollState(Off);
    ui->outputWidget->setStatus(tr("Device NOT CONNECTED!"));
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
    beginUpdate();

    emit pointTypeChanged(type);
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
    const quint8 deviceId = ui->lineEditDeviceId->value<int>();
    const auto pointType = ui->comboBoxModbusPointType->currentPointType();
    const auto zeroBasedAddress = displayDefinition().ZeroBasedAddress;
    const auto simAddr = addr - (zeroBasedAddress ? 0 : 1);
    auto simParams = _dataSimulator->simulationParams(pointType, simAddr, deviceId);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        {
            ModbusWriteParams params = { node, addr, value, mode, byteOrder(), codepage(), zeroBasedAddress };
            DialogWriteCoilRegister dlg(params, simParams, displayHexAddresses(), _parent);
            switch(dlg.exec())
            {
                case QDialog::Accepted:
                    _modbusClient.writeRegister(pointType, params, _formId);
                break;

                case 2:
                    if(simParams.Mode == SimulationMode::Off) _dataSimulator->stopSimulation(pointType, simAddr, deviceId);
                    else _dataSimulator->startSimulation(mode, pointType, simAddr, deviceId, simParams);
                break;
            }
        }
        break;

        case QModbusDataUnit::HoldingRegisters:
        {
            ModbusWriteParams params = { node, addr, value, mode, byteOrder(), codepage(), zeroBasedAddress };
            if(mode == DataDisplayMode::Binary)
            {
                DialogWriteHoldingRegisterBits dlg(params, displayHexAddresses(), _parent);
                if(dlg.exec() == QDialog::Accepted)
                    _modbusClient.writeRegister(pointType, params, _formId);
            }
            else
            {
                DialogWriteHoldingRegister dlg(params, simParams, displayHexAddresses(), _parent);
                switch(dlg.exec())
                {
                    case QDialog::Accepted:
                        _modbusClient.writeRegister(pointType, params, _formId);
                    break;

                    case 2:
                        if(simParams.Mode == SimulationMode::Off) _dataSimulator->stopSimulation(pointType, simAddr, deviceId);
                        else _dataSimulator->startSimulation(mode, pointType, simAddr, deviceId, simParams);
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
    if(_modbusClient.state() != QModbusDevice::ConnectedState)
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
        const ModbusWriteParams params = { dd.DeviceId, addr, value, mode, byteOrder(), codepage(), true };
        _modbusClient.writeRegister(type, params, formId());
    }
}
