#include <QEvent>
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
    ,_mm(nullptr)
{
    setItemDelegate(new HtmlDelegate(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
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
/// \brief ModbusMessageWidget::modbusMessage
/// \return
///
const ModbusMessage* ModbusMessageWidget::modbusMessage() const
{
    return _mm;
}

///
/// \brief ModbusMessageWidget::setModbusMessage
/// \param msg
///
void ModbusMessageWidget::setModbusMessage(const ModbusMessage* msg)
{
    _mm = msg;
    update();
}

///
/// \brief ModbusMessageWidget::update
///
void ModbusMessageWidget::update()
{
    QListWidget::clear();

    if(_mm == nullptr)
        return;

    if(!_mm->isValid())
    {
        if(_mm->isRequest())
            addItem(tr("<span style='color:%1'>*** INVALID MODBUS REQUEST ***</span>").arg(_statusClr.name()));
        else if(!_mm->isException())
            addItem(tr("<span style='color:%1'>*** INVALID MODBUS RESPONSE ***</span>").arg(_statusClr.name()));
    }

    auto addChecksum = [&]{
        if(_mm->protocolType() == ModbusMessage::Rtu)
        {
            auto adu = reinterpret_cast<const QModbusAduRtu*>(_mm->adu());
            const auto checksum = formatUInt16Value(_dataDisplayMode, adu->checksum());
            if(adu->matchingChecksum())
            {
                addItem(tr("<b>Checksum:</b> %1").arg(checksum));
            }
            else
            {
                const auto calcChecksum = formatUInt16Value(_dataDisplayMode, adu->calcChecksum());
                addItem(tr("<b>Checksum:</b> <span style='color:%3'>%1</span> (Expected: %2)").arg(checksum, calcChecksum, _statusClr.name()));
            }
        }
    };

    addItem(tr("<b>Type:</b> %1").arg(_mm->isRequest() ? tr("Request (Tx)") : tr("Response (Rx)")));
    if(_showTimestamp) addItem(tr("<b>Timestamp:</b> %1").arg(_mm->timestamp().toString(Qt::ISODateWithMs)));

    if(_mm->protocolType() == ModbusMessage::Tcp)
    {
        auto adu = reinterpret_cast<const QModbusAduTcp*>(_mm->adu());
        const auto transactionId = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->transactionId()) : "??";
        const auto protocolId = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->protocolId()): "??";
        const auto length = adu->isValid() ? formatUInt16Value(_dataDisplayMode, adu->length()): "??";
        addItem(tr("<b>Transaction ID:</b> %1").arg(transactionId));
        addItem(tr("<b>Protocol ID:</b> %1").arg(protocolId));
        addItem(tr("<b>Length:</b> %1").arg(length));
    }
    
    addItem(tr("<b>Device ID:</b> %1").arg(formatUInt8Value(_dataDisplayMode, _mm->deviceId())));

    if(_mm->isException())
    {
        const auto exception = QString("%1 (%2)").arg(formatUInt8Value(_dataDisplayMode, _mm->exception()), _mm->exception());
        addItem(tr("<b>Error Code:</b> %1").arg(formatUInt8Value(_dataDisplayMode, _mm->function())));
        addItem(tr("<b>Exception Code:</b> %1").arg(exception));
        addChecksum();
        return;
    }

    const auto func = _mm->function();
    const auto function = func.isValid() ?
                              QString("%1 (%2)").arg(formatUInt8Value(_dataDisplayMode, func), func) :
                              formatUInt8Value(_dataDisplayMode, func);
    addItem(tr("<b>Function Code:</b> %1").arg(function));
    const auto addrBase = tr("(0-based)");

    switch(_mm->function())
    {
        case QModbusPdu::ReadCoils:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadCoilsRequest*>(_mm);
            const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1 %2").arg(startAddress, addrBase));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadCoilsResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto coilStatus = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->coilStatus()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Coil Status:</b> %1").arg(coilStatus));
            }
        break;

        case QModbusPdu::ReadDiscreteInputs:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadDiscreteInputsRequest*>(_mm);
            const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1 %2").arg(startAddress, addrBase));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadDiscreteInputsResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto inputStatus = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->inputStatus()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Input Status:</b> %1").arg(inputStatus));
            }
        break;

        case QModbusPdu::ReadHoldingRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadHoldingRegistersRequest*>(_mm);
            const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1 %2").arg(startAddress, addrBase));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadHoldingRegistersResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
        break;

        case QModbusPdu::ReadInputRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadInputRegistersRequest*>(_mm);
            const auto startAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto length = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->length()): "??";
                addItem(tr("<b>Start Address:</b> %1 %2").arg(startAddress, addrBase));
                addItem(tr("<b>Length:</b> %1").arg(length));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadInputRegistersResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto registerValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->registerValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Input Registers:</b> %1").arg(registerValue));
            }
        break;

        case QModbusPdu::WriteSingleCoil:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleCoilRequest*>(_mm);
            const auto outputAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) : "??";
                const auto outputValue = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->value()) : "??";
                addItem(tr("<b>Output Address:</b> %1 %2").arg(outputAddress, addrBase));
                addItem(tr("<b>Output Value:</b> %1").arg(outputValue));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleCoilResponse*>(_mm);
                const auto outputAddress = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) : "??";
                const auto outputValue = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->value()) : "??";
                addItem(tr("<b>Output Address:</b> %1 %2").arg(outputAddress, addrBase));
                addItem(tr("<b>Output Value:</b> %1").arg(outputValue));
            }
        break;

        case QModbusPdu::WriteSingleRegister:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleRegisterRequest*>(_mm);
            const auto registerAddress = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) : "??";
                const auto registerValue = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->value()) : "??";
                addItem(tr("<b>Register Address:</b> %1 %2").arg(registerAddress, addrBase));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleRegisterResponse*>(_mm);
                const auto registerAddress = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) : "??";
                const auto registerValue = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->value()) : "??";
                addItem(tr("<b>Register Address:</b> %1 %2").arg(registerAddress, addrBase));
                addItem(tr("<b>Register Value:</b> %1").arg(registerValue));
            }
            break;

        case QModbusPdu::ReadExceptionStatus:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const ReadExceptionStatusResponse*>(_mm);
                const auto outputData = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->outputData()) : "?";
                addItem(tr("<b>Output Data:</b> %1").arg(outputData));
            }
            break;

        case QModbusPdu::Diagnostics:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const DiagnosticsRequest*>(_mm);
                const auto subFunc = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->subfunc()) : "??";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "???";
                addItem(tr("<b>Sub-function:</b> %1").arg(subFunc));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const DiagnosticsResponse*>(_mm);
                const auto subFunc = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->subfunc()) : "??";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "???";
                addItem(tr("<b>Sub-function:</b> %1").arg(subFunc));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            break;

        case QModbusPdu::GetCommEventCounter:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventCounterResponse*>(_mm);
                const auto status = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->eventCount()) : "??";
                addItem(tr("<b>Status:</b> %1").arg(status));
                addItem(tr("<b>Event Count:</b> %1").arg(eventCount));
            }
            break;

        case QModbusPdu::GetCommEventLog:
            if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventLogResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto status = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->status()) : "??";
                const auto eventCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->eventCount()) : "??";
                const auto messageCount = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->messageCount()) : "??";
                const auto events = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->events()) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Status:</b> %1").arg(status));
                addItem(tr("<b>Event Count:</b> %1").arg(eventCount));
                addItem(tr("<b>Message Count:</b> %1").arg(messageCount));
                addItem(tr("<b>Events:</b> %1").arg(events));
            }
            break;

        case QModbusPdu::WriteMultipleCoils:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleCoilsRequest*>(_mm);
                const auto startAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto quantity = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->values()) : "???";
                addItem(tr("<b>Starting Address:</b> %1 %2").arg(startAddr, addrBase));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(quantity));
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Output Value:</b> %1").arg(values));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleCoilsResponse*>(_mm);
                const auto startAddr = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->startAddress()) : "??";
                const auto quantity = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->quantity()) : "??";
                addItem(tr("<b>Starting Address:</b> %1 %2").arg(startAddr, addrBase));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(quantity));
            }
            break;

        case QModbusPdu::WriteMultipleRegisters:
            if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleRegistersRequest*>(_mm);
                const auto startAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->startAddress()) : "??";
                const auto quantity = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->quantity()) : "??";
                const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto values = req->isValid() ? formatUInt16Array(_dataDisplayMode, req->values(), _byteOrder) : "???";
                addItem(tr("<b>Starting Address:</b> %1 %2").arg(startAddr, addrBase));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(quantity));
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Registers Value:</b> %1").arg(values));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleRegistersResponse*>(_mm);
                const auto startAddr = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->startAddress()) : "??";
                const auto quantity = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->quantity()) : "??";
                addItem(tr("<b>Starting Address:</b> %1 %2").arg(startAddr, addrBase));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(quantity));
            }
        break;

        case QModbusPdu::ReportServerId:
        if(!_mm->isRequest())
            {
                auto resp = reinterpret_cast<const ReportServerIdResponse*>(_mm);
            const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::ReadFileRecord:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadFileRecordRequest*>(_mm);
            const auto byteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->byteCount()) : "?";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFileRecordResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "?";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::WriteFileRecord:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const WriteFileRecordRequest*>(_mm);
            const auto length = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->length()) : "?";
                const auto data = req->isValid() ? formatUInt8Array(_dataDisplayMode, req->data()) : "???";
                addItem(tr("<b>Request Data Length:</b> %1").arg(length));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteFileRecordResponse*>(_mm);
                const auto length = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->length()) : "?";
                const auto data = resp->isValid() ? formatUInt8Array(_dataDisplayMode, resp->data()) : "???";
                addItem(tr("<b>Response Data Length:</b> %1").arg(length));
                addItem(tr("<b>Data:</b> %1").arg(data));
            }
        break;

        case QModbusPdu::MaskWriteRegister:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const MaskWriteRegisterRequest*>(_mm);
            const auto address = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->address()) : "??";
                const auto andMask = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->andMask()) : "??";
            const auto orMask = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->orMask()) : "??";
                addItem(tr("<b>Address:</b> %1 %2").arg(address, addrBase));
                addItem(tr("<b>And Mask:</b> %1").arg(andMask));
                addItem(tr("<b>Or Mask:</b> %1").arg(orMask));
            }
            else
            {
                auto resp = reinterpret_cast<const MaskWriteRegisterResponse*>(_mm);
                const auto address = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->address()) : "??";
                const auto andMask = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->andMask()) : "??";
                const auto orMask = resp->isValid() ? formatUInt16Value(_dataDisplayMode, resp->orMask()) : "??";
                addItem(tr("<b>Address:</b> %1 %2").arg(address, addrBase));
                addItem(tr("<b>And Mask:</b> %1").arg(andMask));
                addItem(tr("<b>Or Mask:</b> %1").arg(orMask));
            }
        break;

        case QModbusPdu::ReadWriteMultipleRegisters:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadWriteMultipleRegistersRequest*>(_mm);
            const auto readStartAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->readStartAddress()) : "??";
                const auto readLength = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->readLength()) : "??";
            const auto writeStartAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->writeStartAddress()) : "??";
                const auto writeLength = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->writeLength()) : "??";
                const auto writeByteCount = req->isValid() ? formatUInt8Value(_dataDisplayMode, req->writeByteCount()) : "?";
                const auto writeValues = req->isValid() ? formatUInt16Array(_dataDisplayMode, req->writeValues(), _byteOrder) : "???";
                addItem(tr("<b>Read Starting Address:</b> %1 %2").arg(readStartAddr, addrBase));
                addItem(tr("<b>Quantity to Read:</b> %1").arg(readLength));
                addItem(tr("<b>Write Starting Address:</b> %1 %2").arg(writeStartAddr, addrBase));
                addItem(tr("<b>Quantity to Write:</b> %1").arg(writeLength));
                addItem(tr("<b>Write Byte Count:</b> %1").arg(writeByteCount));
                addItem(tr("<b>Write Registers Value:</b> %1").arg(writeValues));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadWriteMultipleRegistersResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()): "?";
                const auto values = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->values(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>Registers Value:</b> %1").arg(values));
            }
        break;

        case QModbusPdu::ReadFifoQueue:
        if(_mm->isRequest())
            {
                auto req = reinterpret_cast<const ReadFifoQueueRequest*>(_mm);
            const auto fifoAddr = req->isValid() ? formatUInt16Value(_dataDisplayMode, req->fifoAddress()) : "??";
                addItem(tr("<b>FIFO Point Address:</b> %1 %2").arg(fifoAddr, addrBase));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFifoQueueResponse*>(_mm);
                const auto byteCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->byteCount()) : "?";
                const auto fifoCount = resp->isValid() ? formatUInt8Value(_dataDisplayMode, resp->fifoCount()) : "?";
                const auto fifoValue = resp->isValid() ? formatUInt16Array(_dataDisplayMode, resp->fifoValue(), _byteOrder) : "???";
                addItem(tr("<b>Byte Count:</b> %1").arg(byteCount));
                addItem(tr("<b>FIFO Count:</b> %1").arg(fifoCount));
                addItem(tr("<b>FIFO Value Register:</b> %1").arg(fifoValue));
            }
        break;

        default:
        {
        const auto data = _mm->isValid() ? formatUInt8Array(_dataDisplayMode, _mm->rawData()) : "???";
            addItem(tr("<b>Data:</b> %1").arg(data));
        }
        break;
    }

    addChecksum();
}
