#include "formatutils.h"
#include "htmldelegate.h"
#include "modbusmessagewidget.h"
#include "modbusmessages.h"

///
/// \brief ModbusMessageWidget::ModbusMessageWidget
/// \param parent
///
ModbusMessageWidget::ModbusMessageWidget(QWidget *parent)
    : QListWidget(parent)
    ,_msg(nullptr)
{
    setItemDelegate(new HtmlDelegate(this));
}

///
/// \brief ModbusMessageWidget::clear
///
void ModbusMessageWidget::clear()
{
    _msg = nullptr;
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
/// \brief ModbusMessageWidget::modbusMessage
/// \return
///
const ModbusMessage* ModbusMessageWidget::modbusMessage() const
{
    return _msg;
}

///
/// \brief ModbusMessageWidget::setModbusMessage
/// \param msg
///
void ModbusMessageWidget::setModbusMessage(const ModbusMessage* msg)
{
    _msg = msg;
    update();
}

///
/// \brief ModbusMessageWidget::update
///
void ModbusMessageWidget::update()
{
    QListWidget::clear();
    if(_msg == nullptr)
        return;

    const auto function = QString("%1 (%2)").arg(formatByteValue(_dataDisplayMode, _msg->function()), _msg->function());

    addItem(tr("<b>Type:</b> %1").arg(_msg->isRequest() ? tr("Tx Message") : tr("Rx Message")));
    addItem(tr("<b>Timestamp:</b> %1").arg(_msg->timestamp().toString(Qt::ISODateWithMs)));
    addItem(tr("<b>Device ID:</b> %1").arg(formatByteValue(_dataDisplayMode, _msg->deviceId())));

    if(_msg->isException())
    {
        const auto exception = QString("%1 (%2)").arg(formatByteValue(_dataDisplayMode, _msg->exception()), _msg->exception());
        addItem(tr("<b>Error Code:</b> %1").arg(formatByteValue(_dataDisplayMode, _msg->function())));
        addItem(tr("<b>Exception Code:</b> %1").arg(exception));
        return;
    }

    addItem(tr("<b>Function Code:</b> %1").arg(function));

    switch(_msg->function())
    {
        case QModbusPdu::ReadCoils:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadCoilsRequest*>(_msg);
                addItem(tr("<b>Start Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Length:</b> %1").arg(formatWordValue(_dataDisplayMode, req->length())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadCoilsResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Coil Status:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->coilStatus())));
            }
        break;

        case QModbusPdu::ReadDiscreteInputs:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadDiscreteInputsRequest*>(_msg);
                addItem(tr("<b>Start Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Length:</b> %1").arg(formatWordValue(_dataDisplayMode, req->length())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadDiscreteInputsResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Input Status:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->inputStatus())));
            }
        break;

        case QModbusPdu::ReadHoldingRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadHoldingRegistersRequest*>(_msg);
                addItem(tr("<b>Start Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Length:</b> %1").arg(formatWordValue(_dataDisplayMode, req->length())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadHoldingRegistersResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Register Value:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->registerValue())));
            }
        break;

        case QModbusPdu::ReadInputRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadInputRegistersRequest*>(_msg);
                addItem(tr("<b>Start Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Length:</b> %1").arg(formatWordValue(_dataDisplayMode, req->length())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadInputRegistersResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Input Registers:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->registerValue())));
            }
        break;

        case QModbusPdu::WriteSingleCoil:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleCoilRequest*>(_msg);
                addItem(tr("<b>Output Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->address())));
                addItem(tr("<b>Output Value:</b> %1").arg(formatWordValue(_dataDisplayMode, req->value())));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleCoilResponse*>(_msg);
                addItem(tr("<b>Output Address:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->address())));
                addItem(tr("<b>Output Value:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->value())));
            }
        break;

        case QModbusPdu::WriteSingleRegister:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteSingleRegisterRequest*>(_msg);
                addItem(tr("<b>Register Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->address())));
                addItem(tr("<b>Register Value:</b> %1").arg(formatWordValue(_dataDisplayMode, req->value())));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteSingleRegisterResponse*>(_msg);
                addItem(tr("<b>Register Address:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->address())));
                addItem(tr("<b>Register Value:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->value())));
            }
            break;

        case QModbusPdu::ReadExceptionStatus:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const ReadExceptionStatusResponse*>(_msg);
                addItem(tr("<b>Output Data:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->outputData())));
            }
            break;

        case QModbusPdu::Diagnostics:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const DiagnosticsRequest*>(_msg);
                addItem(tr("<b>Sub-function:</b> %1").arg(formatWordValue(_dataDisplayMode, req->subfunc())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, req->data())));
            }
            else
            {
                auto resp = reinterpret_cast<const DiagnosticsResponse*>(_msg);
                addItem(tr("<b>Sub-function:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->subfunc())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->data())));
            }
            break;

        case QModbusPdu::GetCommEventCounter:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventCounterResponse*>(_msg);
                addItem(tr("<b>Status:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->status())));
                addItem(tr("<b>Event Count:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->eventCount())));
            }
            break;

        case QModbusPdu::GetCommEventLog:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const GetCommEventLogResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Status:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->status())));
                addItem(tr("<b>Event Count:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->eventCount())));
                addItem(tr("<b>Message Count:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->messageCount())));
                addItem(tr("<b>Events:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->events())));
            }
            break;

        case QModbusPdu::WriteMultipleCoils:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleCoilsRequest*>(_msg);
                addItem(tr("<b>Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(formatWordValue(_dataDisplayMode, req->quantity())));
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, req->byteCount())));
                addItem(tr("<b>Output Value:</b> %1").arg(formatByteArray(_dataDisplayMode, req->values())));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleCoilsResponse*>(_msg);
                addItem(tr("<b>Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->startAddress())));
                addItem(tr("<b>Quantity of Outputs:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->quantity())));
            }
            break;

        case QModbusPdu::WriteMultipleRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteMultipleRegistersRequest*>(_msg);
                addItem(tr("<b>Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->startAddress())));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(formatWordValue(_dataDisplayMode, req->quantity())));
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, req->byteCount())));
                addItem(tr("<b>Registers Value:</b> %1").arg(formatByteArray(_dataDisplayMode, req->values())));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteMultipleRegistersResponse*>(_msg);
                addItem(tr("<b>Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->startAddress())));
                addItem(tr("<b>Quantity of Registers:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->quantity())));
            }
        break;

        case QModbusPdu::ReportServerId:
            if(!_msg->isRequest())
            {
                auto resp = reinterpret_cast<const ReportServerIdResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->data())));
            }
        break;

        case QModbusPdu::ReadFileRecord:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadFileRecordRequest*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, req->byteCount())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, req->data())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFileRecordResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->data())));
            }
        break;

        case QModbusPdu::WriteFileRecord:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const WriteFileRecordRequest*>(_msg);
                addItem(tr("<b>Request _msg Length:</b> %1").arg(formatByteValue(_dataDisplayMode, req->length())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, req->data())));
            }
            else
            {
                auto resp = reinterpret_cast<const WriteFileRecordResponse*>(_msg);
                addItem(tr("<b>Request _msg Length:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->length())));
                addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->data())));
            }
        break;

        case QModbusPdu::MaskWriteRegister:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const MaskWriteRegisterRequest*>(_msg);
                addItem(tr("<b>Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->address())));
                addItem(tr("<b>And Mask:</b> %1").arg(formatWordValue(_dataDisplayMode, req->andMask())));
                addItem(tr("<b>Or Mask:</b> %1").arg(formatWordValue(_dataDisplayMode, req->orMask())));
            }
            else
            {
                auto resp = reinterpret_cast<const MaskWriteRegisterResponse*>(_msg);
                addItem(tr("<b>Address:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->address())));
                addItem(tr("<b>And Mask:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->andMask())));
                addItem(tr("<b>Or Mask:</b> %1").arg(formatWordValue(_dataDisplayMode, resp->orMask())));
            }
        break;

        case QModbusPdu::ReadWriteMultipleRegisters:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadWriteMultipleRegistersRequest*>(_msg);
                addItem(tr("<b>Read Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->readStartAddress())));
                addItem(tr("<b>Quantity to Read:</b> %1").arg(formatWordValue(_dataDisplayMode, req->readLength())));
                addItem(tr("<b>Write Starting Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->writeStartAddress())));
                addItem(tr("<b>Quantity to Write:</b> %1").arg(formatWordValue(_dataDisplayMode, req->writeLength())));
                addItem(tr("<b>Write Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, req->writeByteCount())));
                addItem(tr("<b>Write Registers Value:</b> %1").arg(formatByteArray(_dataDisplayMode, req->writeValues())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadWriteMultipleRegistersResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>Registers Value:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->values())));
            }
        break;

        case QModbusPdu::ReadFifoQueue:
            if(_msg->isRequest())
            {
                auto req = reinterpret_cast<const ReadFifoQueueRequest*>(_msg);
                addItem(tr("<b>FIFO Point Address:</b> %1").arg(formatWordValue(_dataDisplayMode, req->fifoAddress())));
            }
            else
            {
                auto resp = reinterpret_cast<const ReadFifoQueueResponse*>(_msg);
                addItem(tr("<b>Byte Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->byteCount())));
                addItem(tr("<b>FIFO Count:</b> %1").arg(formatByteValue(_dataDisplayMode, resp->fifoCount())));
                addItem(tr("<b>FIFO Value Register:</b> %1").arg(formatByteArray(_dataDisplayMode, resp->fifoValue())));
            }
        break;

        default:
            addItem(tr("<b>Data:</b> %1").arg(formatByteArray(_dataDisplayMode, _msg->rawData())));
        break;
    }
}
