#include <QMenu>
#include <QEvent>
#include <QClipboard>
#include <QApplication>
#include <QTextDocument>
#include "fontutils.h"
#include "formatutils.h"
#include "htmldelegate.h"
#include "modbusmessages.h"
#include "modbusmessagewidget.h"

///
/// \brief ModbusMessageWidget::ModbusMessageWidget
/// \param parent
///
ModbusMessageWidget::ModbusMessageWidget(QWidget *parent)
    : QListWidget(parent)
    ,_statusClr(Qt::red)
    ,_byteOrder(ByteOrder::Direct)
    ,_dataDisplayMode(DataDisplayMode::UInt16)
    ,_showTimestamp(true)
{
    setFocusPolicy(Qt::NoFocus);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    setItemDelegate(new HtmlDelegate(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFont(defaultMonospaceFont());

    QIcon copyIcon = QIcon::fromTheme("edit-copy");
    if (copyIcon.isNull()) {
        copyIcon = style()->standardIcon(QStyle::SP_FileIcon);
    }

    _copyAct = new QAction(copyIcon, tr("Copy Text"), this);
    addAction(_copyAct);

    connect(_copyAct, &QAction::triggered, this, [this]() {
        QString text;
        for (int i = 0; i < count(); ++i) {
            QListWidgetItem* item = this->item(i);
            if (item) {
                QTextDocument doc;
                doc.setHtml(item->text());
                text += doc.toPlainText() + "\n";
            }
        }
        QApplication::clipboard()->setText(text.trimmed());
    });

    _copyValuesAct = new QAction(tr("Copy Values"), this);
    addAction(_copyValuesAct);

    connect(_copyValuesAct, &QAction::triggered, this, [this]() {
        QString allValues;
        switch(_mm->function()) {
        case QModbusPdu::ReadCoils: {
            auto resp = reinterpret_cast<const ReadCoilsResponse*>(_mm.get());
            allValues = formatUInt8Array(_dataDisplayMode, resp->coilStatus());
        } break;
        case QModbusPdu::ReadDiscreteInputs: {
            auto resp = reinterpret_cast<const ReadDiscreteInputsResponse*>(_mm.get());
            allValues = formatUInt8Array(_dataDisplayMode, resp->inputStatus());
        } break;
        case QModbusPdu::ReadHoldingRegisters: {
            auto resp = reinterpret_cast<const ReadHoldingRegistersResponse*>(_mm.get());
            allValues = formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder);
        } break;
        case QModbusPdu::ReadInputRegisters: {
            auto resp = reinterpret_cast<const ReadInputRegistersResponse*>(_mm.get());
            allValues = formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder);
        } break;
        default: break;
        }
        if(!allValues.isEmpty()) QApplication::clipboard()->setText(allValues);
    });

    connect(this, &QWidget::customContextMenuRequested,
            this, &ModbusMessageWidget::on_customContextMenuRequested);
}

///
/// \brief ModbusMessageWidget::changeEvent
/// \param event
///
void ModbusMessageWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        update();
        _copyAct->setText(tr("Copy Text"));
        _copyValuesAct->setText(tr("Copy Values"));
    }
    QListWidget::changeEvent(event);
}

///
/// \brief ModbusMessageWidget::clear
///
void ModbusMessageWidget::clear()
{
    _mm = nullptr;
    update();
}

///
/// \brief ModbusMessageWidget::dataDisplayMode
/// \return
///
DataDisplayMode ModbusMessageWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief ModbusMessageWidget::setDataDisplayMode
/// \param mode
///
void ModbusMessageWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    update();
}

///
/// \brief ModbusMessageWidget::byteOrder
/// \return
///
ByteOrder ModbusMessageWidget::byteOrder() const
{
    return _byteOrder;
}

///
/// \brief ModbusMessageWidget::setByteOrder
/// \param order
///
void ModbusMessageWidget::setByteOrder(ByteOrder order)
{
    _byteOrder = order;
    update();
}

///
/// \brief ModbusMessageWidget::showTimestamp
/// \return
///
bool ModbusMessageWidget::showTimestamp() const
{
    return _showTimestamp;
}

///
/// \brief ModbusMessageWidget::setShowTimestamp
/// \param on
///
void ModbusMessageWidget::setShowTimestamp(bool on)
{
    _showTimestamp = on;
    update();
}

///
/// \brief ModbusMessageWidget::setStatusColor
/// \param clr
///
void ModbusMessageWidget::setStatusColor(const QColor& clr)
{
    _statusClr = clr;
    update();
}

///
/// \brief ModbusMessageWidget::backgroundColor
/// \return
///
QColor ModbusMessageWidget::backgroundColor() const
{
    return palette().color(QPalette::Base);
}

///
/// \brief ModbusLogWidget::setBackGroundColor
/// \param clr
///
void ModbusMessageWidget::setBackGroundColor(const QColor& clr)
{
    auto pal = palette();
    pal.setColor(QPalette::Base, clr);
    pal.setColor(QPalette::Window, clr);
    setPalette(pal);
}

///
/// \brief ModbusMessageWidget::modbusMessage
/// \return
///
QSharedPointer<const ModbusMessage> ModbusMessageWidget::modbusMessage() const
{
    return _mm;
}

///
/// \brief ModbusMessageWidget::setModbusMessage
/// \param msg
///
void ModbusMessageWidget::setModbusMessage(QSharedPointer<const ModbusMessage> msg)
{
    _mm = msg;
    update();
}

///
/// \brief ModbusMessageWidget::update
///
void ModbusMessageWidget::update()
{
    static const QString errColor = "#cc0000";
    static const QString valueColor = "#663399";
    static const QString dataColor = "#444444";

    QListWidget::clear();

    if(_mm == nullptr)
        return;

    if(!_mm->isValid())
    {
        if(_mm->isRequest())
            addItem(tr("<span style='color:%1'>*** INVALID MODBUS REQUEST ***</span>").arg(errColor));
        else if(!_mm->isException())
            addItem(tr("<span style='color:%1'>*** INVALID MODBUS RESPONSE ***</span>").arg(errColor));
    }

    const QString dirColor = _mm->isRequest() ? "#0066cc" : "#009933";
    auto addField = [&](const QString &name, const QString &value, const QString &clr) {
        addItem(QString("<b style='color:#000000'>%1:</b> <span style='color:%2'>%3</span>").arg(name, clr, value));
    };

    auto addChecksum = [&]{
        if(_mm->protocolType() == ModbusMessage::Rtu)
        {
            auto adu = reinterpret_cast<const QModbusAduRtu*>(_mm->adu());
            const auto checksum = formatUInt16Value(_dataDisplayMode, adu->checksum());
            if(adu->matchingChecksum())
            {
                addField(tr("Checksum"), checksum, dataColor);
            }
            else
            {
                const auto calcChecksum = formatUInt16Value(_dataDisplayMode, adu->calcChecksum());
                addField(tr("Checksum"), QString("%1 <span style='color:#000000'>(Expected: %2)</span>").arg(checksum, calcChecksum), errColor);
            }
        }
    };

    addField(tr("Type"), _mm->isRequest() ? tr("Request (Tx)") : tr("Response (Rx)"), dirColor);
    if(_showTimestamp) addField(tr("Time"), _mm->timestamp().toString(Qt::ISODateWithMs), dataColor);

    if(_mm->protocolType() == ModbusMessage::Tcp)
    {
        auto adu = reinterpret_cast<const QModbusAduTcp*>(_mm->adu());
        const auto transactionId = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->transactionId()) : "??";
        const auto protocolId = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->protocolId()): "??";
        const auto length = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->length()): "??";
        addField(tr("Transaction ID"), transactionId, valueColor);
        addField(tr("Protocol ID"), protocolId, valueColor);
        addField(tr("Length"), length, valueColor);
    }
    
    addField(tr("Device ID"), formatUInt8Value(_dataDisplayMode, _mm->deviceId()), valueColor);

    if(_mm->isException())
    {
        const auto exception = QString("%1 (%2)").arg(formatUInt8Value(_dataDisplayMode, _mm->exception()), _mm->exception());        
        addField(tr("Error Code"), formatUInt8Value(_dataDisplayMode, _mm->function()), errColor);
        addField(tr("Exception Code"), exception, errColor);
        addChecksum();
        return;
    }

    const auto func = _mm->function();
    const auto function = func.isValid() ?
                              QString("%1 (%2)").arg(formatUInt8Value(_dataDisplayMode, func), func) :
                              formatUInt8Value(_dataDisplayMode, func);
    addField(tr("Function Code"), function, dirColor);
    const auto addrBase = tr("(0-based)");

    switch(_mm->function())
    {
        case QModbusPdu::ReadCoils:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadCoilsRequest*>(_mm.get());
                const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addField(tr("Start Address"), startAddress, dataColor);
                addField(tr("Length"), length, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadCoilsResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto coilStatus = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->coilStatus()) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Coil Status"), coilStatus, dataColor);
            }
        break;

        case QModbusPdu::ReadDiscreteInputs:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadDiscreteInputsRequest*>(_mm.get());
                const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addField(tr("Start Address"), startAddress, dataColor);
                addField(tr("Length"), length, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadDiscreteInputsResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto inputStatus = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->inputStatus()) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Input Status"), inputStatus, dataColor);
            }
        break;

        case QModbusPdu::ReadHoldingRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadHoldingRegistersRequest*>(_mm.get());
                const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addField(tr("Start Address"), startAddress, dataColor);
                addField(tr("Length"), length, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadHoldingRegistersResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Register Value"), registerValue, dataColor);
            }
        break;

        case QModbusPdu::ReadInputRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadInputRegistersRequest*>(_mm.get());
                const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addField(tr("Start Address"), startAddress, dataColor);
                addField(tr("Length"), length, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadInputRegistersResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Input Registers"), registerValue, dataColor);
            }
        break;

        case QModbusPdu::WriteSingleCoil:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleCoilRequest*>(_mm.get());
                const auto outputAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) + QString(" %1").arg(addrBase) : "??";
                const auto outputValue = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->value()) : "??";
                addField(tr("Output Address"), outputAddress, dataColor);
                addField(tr("Output Value"), outputValue, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleCoilResponse*>(_mm.get());
                const auto outputAddress = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) + QString(" %1").arg(addrBase) : "??";
                const auto outputValue = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->value()) : "??";
                addField(tr("Output Address"), outputAddress, dataColor);
                addField(tr("Output Value"), outputValue, dataColor);
            }
        break;

        case QModbusPdu::WriteSingleRegister:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleRegisterRequest*>(_mm.get());
                const auto registerAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) + QString(" %1").arg(addrBase): "??";
                const auto registerValue = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->value()) : "??";
                addField(tr("Register Address"), registerAddress, dataColor);
                addField(tr("Register Value"), registerValue, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleRegisterResponse*>(_mm.get());
                const auto registerAddress = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) + QString(" %1").arg(addrBase) : "??";
                const auto registerValue = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->value()) : "??";
                addField(tr("Register Address"), registerAddress, dataColor);
                addField(tr("Register Value"), registerValue, dataColor);
            }
            break;

        case QModbusPdu::ReadExceptionStatus:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const ReadExceptionStatusResponse*>(_mm.get());
                const auto outputData = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->outputData()) : "?";
                addField(tr("Output Data"), outputData, dataColor);
            }
            break;

        case QModbusPdu::Diagnostics:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const DiagnosticsRequest*>(_mm.get());
                const auto subFunc = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->subfunc()) : "??";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "???";
                addField(tr("Sub-function"), subFunc, dataColor);
                addField(tr("Data"), data, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const DiagnosticsResponse*>(_mm.get());
                const auto subFunc = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->subfunc()) : "??";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "???";
                addField(tr("Sub-function"), subFunc, dataColor);
                addField(tr("Data"), data, dataColor);
            }
            break;

        case QModbusPdu::GetCommEventCounter:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventCounterResponse*>(_mm.get());
                const auto status = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->eventCount()) : "??";
                addField(tr("Status"), status, dataColor);
                addField(tr("Event Count"), eventCount, dataColor);
            }
            break;

        case QModbusPdu::GetCommEventLog:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventLogResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto status = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->eventCount()) : "??";
                const auto messageCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->messageCount()) : "??";
                const auto events = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->events()) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Status"), status, dataColor);
                addField(tr("Event Count"), eventCount, dataColor);
                addField(tr("Message Count"), messageCount, dataColor);
                addField(tr("Events"), events, dataColor);
            }
            break;

        case QModbusPdu::WriteMultipleCoils:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleCoilsRequest*>(_mm.get());
                const auto startAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto quantity = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->values()) : "???";
                addField(tr("Starting Address"), startAddr, dataColor);
                addField(tr("Quantity of Outputs"), quantity, dataColor);
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Outputs Value"), values, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleCoilsResponse*>(_mm.get());
                const auto startAddr = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto quantity = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->quantity()) : "??";
                addField(tr("Starting Address"), startAddr, dataColor);
                addField(tr("Quantity of Outputs"), quantity, dataColor);
            }
            break;

        case QModbusPdu::WriteMultipleRegisters:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleRegistersRequest*>(_mm.get());
                const auto startAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto quantity = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatUInt16Array(_dataDisplayMode, req->values(), _byteOrder) : "???";
                addField(tr("Starting Address"), startAddr, dataColor);
                addField(tr("Quantity of Registers"), quantity, dataColor);
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Registers Value"), values, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleRegistersResponse*>(_mm.get());
                const auto startAddr = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->startAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto quantity = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->quantity()) : "??";
                addField(tr("Starting Address"), startAddr, dataColor);
                addField(tr("Quantity of Registers"), quantity, dataColor);
            }
        break;

        case QModbusPdu::ReportServerId:
        if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const ReportServerIdResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "?";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Data"), data, dataColor);
            }
        break;

        case QModbusPdu::ReadFileRecord:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadFileRecordRequest*>(_mm.get());
                const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "?";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Data"), data, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFileRecordResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "?";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Data"), data, dataColor);
            }
        break;

        case QModbusPdu::WriteFileRecord:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteFileRecordRequest*>(_mm.get());
                const auto length = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->length()) : "?";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "???";
                addField(tr("Request Data Length"), length, dataColor);
                addField(tr("Data"), data, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const WriteFileRecordResponse*>(_mm.get());
                const auto length = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->length()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "???";
                addField(tr("Response Data Length"), length, dataColor);
                addField(tr("Data"), data, dataColor);
            }
        break;

        case QModbusPdu::MaskWriteRegister:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const MaskWriteRegisterRequest*>(_mm.get());
                const auto address = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) + QString(" %1").arg(addrBase) : "??";
                const auto andMask = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->andMask()) : "??";
                const auto orMask = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->orMask()) : "??";
                addField(tr("Address"), address, dataColor);
                addField(tr("And Mask"), andMask, dataColor);
                addField(tr("Or Mask"), orMask, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const MaskWriteRegisterResponse*>(_mm.get());
                const auto address = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) + QString(" %1").arg(addrBase): "??";
                const auto andMask = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->andMask()) : "??";
                const auto orMask = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->orMask()) : "??";
                addField(tr("Address"), address, dataColor);
                addField(tr("And Mask"), andMask, dataColor);
                addField(tr("Or Mask"), orMask, dataColor);
            }
        break;

        case QModbusPdu::ReadWriteMultipleRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadWriteMultipleRegistersRequest*>(_mm.get());
                const auto readStartAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->readStartAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto readLength = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->readLength()) : "??";
                const auto writeStartAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->writeStartAddress()) + QString(" %1").arg(addrBase) : "??";
                const auto writeLength = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->writeLength()) : "??";
                const auto writeByteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->writeByteCount()) : "?";
                const auto writeValues = req->isValid() ? formatUInt16Array(_dataDisplayMode, req->writeValues(), _byteOrder) : "???";
                addField(tr("Read Starting Address"), readStartAddr, dataColor);
                addField(tr("Quantity to Read"), readLength, dataColor);
                addField(tr("Write Starting Address"), writeStartAddr, dataColor);
                addField(tr("Quantity to Write"), writeLength, dataColor);
                addField(tr("Write Byte Count"), writeByteCount, dataColor);
                addField(tr("Write Registers Value"), writeValues, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadWriteMultipleRegistersResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()): "?";
                const auto values = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->values(), _byteOrder) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("Registers Value"), values, dataColor);
            }
        break;

        case QModbusPdu::ReadFifoQueue:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadFifoQueueRequest*>(_mm.get());
                const auto fifoAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->fifoAddress()) + QString(" %1").arg(addrBase) : "??";
                addField(tr("FIFO Point Address"), fifoAddr, dataColor);
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFifoQueueResponse*>(_mm.get());
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto fifoCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->fifoCount()) : "?";
                const auto fifoValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->fifoValue(), _byteOrder) : "???";
                addField(tr("Byte Count"), byteCount, dataColor);
                addField(tr("FIFO Count"), fifoCount, dataColor);
                addField(tr("FIFO Value Register"), fifoValue, dataColor);
            }
        break;

        default:
        {
            const auto data = _mm->isValid() ? formatUInt8Array(_dataDisplayMode, _mm->adu()->pdu().data()) : "???";
            if(!data.isEmpty())
                addField(tr("Data"), data, dataColor);
        }
        break;
    }

    addChecksum();
}

///
/// \brief ModbusMessageWidget::on_customContextMenuRequested
/// \param pos
///
void ModbusMessageWidget::on_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    menu.addAction(_copyAct);

    if(!_mm->isRequest() && !_mm->isException())
    {
        switch(_mm->function())
        {
            case QModbusPdu::ReadCoils:
            case QModbusPdu::ReadDiscreteInputs:
            case QModbusPdu::ReadInputRegisters:
            case QModbusPdu::ReadHoldingRegisters:
                menu.addAction(_copyValuesAct);
            break;
            default: break;
        }
    }

    menu.exec(viewport()->mapToGlobal(pos));
}
