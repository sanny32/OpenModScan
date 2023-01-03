#include "modbusexception.h"
#include "outputwidget.h"
#include "ui_outputwidget.h"

struct ListItemData
{
    int Row;
    quint32 Address;
    QVariant Value;
};
Q_DECLARE_METATYPE(ListItemData)

///
/// \brief OutputWidget::OutputWidget
/// \param parent
///
OutputWidget::OutputWidget(QWidget *parent) :
     QWidget(parent)
   , ui(new Ui::OutputWidget)
   ,_displayHexAddreses(false)
   ,_displayMode(DisplayMode::Data)
   ,_dataDisplayMode(DataDisplayMode::Binary)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    setStatus("Data Uninitialized");
}

///
/// \brief OutputWidget::~OutputWidget
///
OutputWidget::~OutputWidget()
{
    delete ui;
}

///
/// \brief OutputWidget::setup
/// \param dd
///
void OutputWidget::setup(const DisplayDefinition& dd)
{
    _displayDefinition = dd;
    updateDataWidget(QModbusDataUnit());
}

///
/// \brief OutputWidget::displayHexAddreses
/// \return
///
bool OutputWidget::displayHexAddreses() const
{
    return _displayHexAddreses;
}

///
/// \brief OutputWidget::setDisplayHexAddreses
/// \param on
///
void OutputWidget::setDisplayHexAddreses(bool on)
{
    _displayHexAddreses = on;
    updateDataWidget(_lastDataResult);
}

///
/// \brief OutputWidget::setStatus
/// \param status
///
void OutputWidget::setStatus(const QString& status)
{
    if(status.isEmpty())
        ui->labelStatus->setText(QString());
    else
        ui->labelStatus->setText(QString("** %1 **").arg(status));
}

///
/// \brief OutputWidget::update
/// \param request
///
void OutputWidget::update(const QModbusRequest& request)
{
    updateTrafficWidget(true, request);
}

///
/// \brief OutputWidget::update
///
void OutputWidget::update(QModbusReply* reply)
{
    if(!reply)
    {
        return;
    }

    const auto raw = reply->rawResult();
    updateTrafficWidget(false, raw);

    const auto fc = raw.functionCode();
    if(fc == QModbusRequest::ReadCoils ||
       fc == QModbusRequest::ReadDiscreteInputs ||
       fc == QModbusRequest::ReadInputRegisters ||
       fc == QModbusRequest::ReadHoldingRegisters)
    {
        const auto data = reply->result();
        if (reply->error() == QModbusDevice::NoError)
        {
            if(data.valueCount() != _displayDefinition.Length ||
               data.startAddress() != _displayDefinition.PointAddress - 1)
            {
                setStatus("Received Invalid Response MODBUS Query");
            }
            else
            {
                setStatus(QString());
                updateDataWidget(data);
                _lastDataResult = data;
            }
        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
            const QString exception = ModbusException(raw.exceptionCode());
            setStatus(exception);
        }
        else
        {
            setStatus(reply->errorString());
        }
    }
}

///
/// \brief OutputWidget::displayMode
/// \return
///
DisplayMode OutputWidget::displayMode() const
{
    return _displayMode;
}

///
/// \brief OutputWidget::setDisplayMode
/// \param mode
///
void OutputWidget::setDisplayMode(DisplayMode mode)
{
    _displayMode = mode;
    switch(mode)
    {
        case DisplayMode::Data:
            ui->stackedWidget->setCurrentIndex(0);
        break;

        case DisplayMode::Traffic:
            ui->stackedWidget->setCurrentIndex(1);
        break;
    }
}

///
/// \brief OutputWidget::dataDisplayMode
/// \return
///
DataDisplayMode OutputWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief OutputWidget::setDataDisplayMode
/// \param mode
///
void OutputWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    updateDataWidget(_lastDataResult);
}

///
/// \brief formatBinatyValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatBinatyValue(QModbusDataUnit::RegisterType pointType, quint16 value, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 16, 2, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatDecimalValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatDecimalValue(QModbusDataUnit::RegisterType pointType, quint16 value, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("<%1>").arg(value, 1, 10, QLatin1Char('0'));
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatIntegerValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatIntegerValue(QModbusDataUnit::RegisterType pointType, qint16 value, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char(' '));
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatHexValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
QString formatHexValue(QModbusDataUnit::RegisterType pointType, quint16 value, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1H>").arg(value, 4, 16, QLatin1Char('0'));
        break;
        default:
        break;
    }
    outValue = value;
    return result.toUpper();
}

///
/// \brief formatFloatValue
/// \param pointType
/// \param value1
/// \param value2
/// \param flag
/// \param outValue
/// \return
///
QString formatFloatValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            union {
               quint16 asUint16[2];
               float asFloat;
            } v;
            v.asUint16[0] = value1;
            v.asUint16[1] = value2;

            outValue = v.asFloat;
            result = QLocale().toString(v.asFloat);
        }
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatDoubleValue
/// \param pointType
/// \param value1
/// \param value2
/// \param value3
/// \param value4
/// \param flag
/// \param outValue
/// \return
///
QString formatDoubleValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, quint16 value3, quint16 value4, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            union {
               quint16 asUint16[4];
               double asDouble;
            } v;
            v.asUint16[0] = value1;
            v.asUint16[1] = value2;
            v.asUint16[2] = value3;
            v.asUint16[3] = value4;

            outValue = v.asDouble;
            result = QLocale().toString(v.asDouble);
        }
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief OutputWidget::on_listWidget_itemDoubleClicked
/// \param item
///
void OutputWidget::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item == nullptr) return;
    auto itemData = item->data(Qt::UserRole).value<ListItemData>();

    switch(_dataDisplayMode)
    {
        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            if(itemData.Row % 2)
            {
                auto item = ui->listWidget->item(itemData.Row - 1);
                if(item) itemData = item->data(Qt::UserRole).value<ListItemData>();
            }
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            if(itemData.Row % 4)
            {
                auto item = ui->listWidget->item(itemData.Row - itemData.Row % 4);
                if(item) itemData = item->data(Qt::UserRole).value<ListItemData>();
            }
        break;

        default:
        break;
    }
    emit itemDoubleClicked(itemData.Address, itemData.Value);
}

///
/// \brief OutputWidget::updateDataWidget
/// \param data
///
void OutputWidget::updateDataWidget(const QModbusDataUnit& data)
{
    ui->listWidget->clear();

    QString prefix;
    if(!_displayHexAddreses)
    {
        switch(_displayDefinition.PointType)
        {
            case QModbusDataUnit::Coils:
                prefix = "0";
            break;
            case QModbusDataUnit::DiscreteInputs:
                prefix = "1";
            break;
            case QModbusDataUnit::HoldingRegisters:
                prefix = "4";
            break;
            case QModbusDataUnit::InputRegisters:
                prefix = "3";
            break;
            default:
            break;
        }
    }

    for(int i = 0; i < _displayDefinition.Length; i++)
    {
        ListItemData itemData;
        itemData.Row = i;
        itemData.Address = i + _displayDefinition.PointAddress;

        const auto addr = _displayHexAddreses ?
                    QStringLiteral("%1H").arg(itemData.Address, 4, 16, QLatin1Char('0')) :
                    QStringLiteral("%1").arg(itemData.Address, 4, 10, QLatin1Char('0'));
        const auto value1 = data.value(i);
        const auto format = "%1%2: %3                  ";

        QString valstr;
        switch(_dataDisplayMode)
        {
            case DataDisplayMode::Binary:
                valstr = formatBinatyValue(_displayDefinition.PointType, value1, itemData.Value);
            break;

            case DataDisplayMode::Decimal:
                valstr = formatDecimalValue(_displayDefinition.PointType, value1, itemData.Value);
            break;

            case DataDisplayMode::Integer:
                valstr = formatIntegerValue(_displayDefinition.PointType, value1, itemData.Value);
            break;

            case DataDisplayMode::Hex:
                valstr = formatHexValue(_displayDefinition.PointType, value1, itemData.Value);
            break;

            case DataDisplayMode::FloatingPt:
            {
                if( i + 1 < _displayDefinition.Length)
                {
                    const auto value2 = data.value(i + 1);
                    valstr = formatFloatValue(_displayDefinition.PointType, value1, value2, i%2, itemData.Value);
                }
            }
            break;

            case DataDisplayMode::SwappedFP:
            {
                if( i + 1 < _displayDefinition.Length)
                {
                    const auto value2 = data.value(i + 1);
                    valstr = formatFloatValue(_displayDefinition.PointType, value2, value1, i%2, itemData.Value);
                }
            }
            break;

            case DataDisplayMode::DblFloat:
            {
                if( i + 3 < _displayDefinition.Length)
                {
                    const auto value2 = data.value(i + 1);
                    const auto value3 = data.value(i + 2);
                    const auto value4 = data.value(i + 3);
                    valstr = formatDoubleValue(_displayDefinition.PointType, value1, value2, value3, value4, i%4, itemData.Value);
                }
            }
            break;

            case DataDisplayMode::SwappedDbl:
            {
                if( i + 3 < _displayDefinition.Length)
                {
                    const auto value2 = data.value(i + 1);
                    const auto value3 = data.value(i + 2);
                    const auto value4 = data.value(i + 3);
                    valstr = formatDoubleValue(_displayDefinition.PointType, value4, value3, value2, value1, i%4, itemData.Value);
                }
            }
            break;
        }

        const auto label = QString(format).arg(prefix, addr, valstr);
        auto item = new QListWidgetItem(label, ui->listWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(itemData));
        ui->listWidget->addItem(item);
    }
}

///
/// \brief OutputWidget::updateTrafficWidget
/// \param request
/// \param pdu
///
void OutputWidget::updateTrafficWidget(bool request, const QModbusPdu& pdu)
{
    if(!pdu.isValid()) return;

    QByteArray rawData;
    rawData.push_back(pdu.functionCode());
    rawData.push_back(pdu.data());

    QString text;
    for(auto&& c : rawData)
    {
        switch(_dataDisplayMode)
        {
            case DataDisplayMode::Decimal:
            case DataDisplayMode::Integer:
                text+= QString("[%1]").arg(QString::number((uchar)c), 3, '0');
            break;

            default:
                text+= QString("[%1]").arg(QString::number((uchar)c, 16), 2, '0');
            break;
        }
    }
    if(text.isEmpty()) return;

    ui->plainTextEdit->moveCursor(QTextCursor::End);

    QTextCharFormat fmt;
    fmt.setForeground(request? Qt::black : Qt::white);
    fmt.setBackground(request? Qt::transparent : Qt::black);
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);

    if(request && ui->plainTextEdit->toPlainText().length() > 22000)
        ui->plainTextEdit->clear();

    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}
