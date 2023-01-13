#include <QDateTime>
#include <QPainter>
#include <QTextStream>
#include "floatutils.h"
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
    ui->labelStatus->setAutoFillBackground(true);

    setAutoFillBackground(true);
    setForegroundColor(Qt::black);
    setBackgroundColor(Qt::lightGray);

    setStatusColor(Qt::red);
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
/// \brief OutputWidget::data
/// \return
///
QVector<quint16> OutputWidget::data() const
{
    return _lastData.values();
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
/// \brief OutputWidget::displayHexAddresses
/// \return
///
bool OutputWidget::displayHexAddresses() const
{
    return _displayHexAddreses;
}

///
/// \brief OutputWidget::setDisplayHexAddresses
/// \param on
///
void OutputWidget::setDisplayHexAddresses(bool on)
{
    _displayHexAddreses = on;
    updateDataWidget(_lastData);
}

///
/// \brief OutputWidget::captureMode
/// \return
///
CaptureMode OutputWidget::captureMode() const
{
    return _fileCapture.isOpen() ? CaptureMode::TextCapture : CaptureMode::Off;
}

///
/// \brief OutputWidget::startTextCapture
/// \param file
///
void OutputWidget::startTextCapture(const QString& file)
{
    _fileCapture.setFileName(file);
    _fileCapture.open(QFile::Text | QFile::WriteOnly);
}

///
/// \brief OutputWidget::stopTextCapture
///
void OutputWidget::stopTextCapture()
{
    if(_fileCapture.isOpen())
        _fileCapture.close();
}

///
/// \brief OutputWidget::backgroundColor
/// \return
///
QColor OutputWidget::backgroundColor() const
{
    return ui->listWidget->palette().color(QPalette::Base);
}

///
/// \brief OutputWidget::setBackgroundColor
/// \param clr
///
void OutputWidget::setBackgroundColor(const QColor& clr)
{
    auto pal = palette();
    pal.setColor(QPalette::Base, clr);
    pal.setColor(QPalette::Window, clr);
    setPalette(pal);
}

///
/// \brief OutputWidget::foregroundColor
/// \return
///
QColor OutputWidget::foregroundColor() const
{
    return ui->listWidget->palette().color(QPalette::Text);
}

///
/// \brief OutputWidget::setForegroundColor
/// \param clr
///
void OutputWidget::setForegroundColor(const QColor& clr)
{
    auto pal = ui->listWidget->palette();
    pal.setColor(QPalette::Text, clr);
    ui->listWidget->setPalette(pal);
}

///
/// \brief OutputWidget::statusColor
/// \return
///
QColor OutputWidget::statusColor() const
{
    return ui->labelStatus->palette().color(QPalette::WindowText);
}

///
/// \brief OutputWidget::setStatusColor
/// \param clr
///
void OutputWidget::setStatusColor(const QColor& clr)
{
    auto pal = ui->labelStatus->palette();
    pal.setColor(QPalette::WindowText, clr);
    ui->labelStatus->setPalette(pal);
}

///
/// \brief OutputWidget::font
/// \return
///
QFont OutputWidget::font() const
{
    return ui->listWidget->font();
}

///
/// \brief OutputWidget::setFont
/// \param font
///
void OutputWidget::setFont(const QFont& font)
{
    ui->listWidget->setFont(font);
    ui->labelStatus->setFont(font);
}

///
/// \brief OutputWidget::setStatus
/// \param status
///
void OutputWidget::setStatus(const QString& status)
{
    if(status.isEmpty())
    {
        ui->labelStatus->setText(status);
    }
    else
    {
        const auto info = QString("** %1 **").arg(status);
        if(info != ui->labelStatus->text())
        {
            ui->labelStatus->setText(info);
            captureString(info);
        }
    }
}

///
/// \brief OutputWidget::paint
/// \param rc
/// \param painter
///
void OutputWidget::paint(const QRect& rc, QPainter& painter)
{
    const auto textStatus = ui->labelStatus->text();
    auto rcStatus = painter.boundingRect(rc.left(), rc.top(), rc.width(), rc.height(), Qt::TextWordWrap, textStatus);
    painter.drawText(rcStatus, Qt::TextWordWrap, textStatus);

    rcStatus.setBottom(rcStatus.bottom() + 4);
    painter.drawLine(rc.left(), rcStatus.bottom(), rc.right(), rcStatus.bottom());
    rcStatus.setBottom(rcStatus.bottom() + 4);

    int cx = rc.left();
    int cy = rcStatus.bottom();
    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        if(!item) continue;

        const auto textItem = item->text().trimmed();
        auto rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, textItem);

        if(rcItem.right() > rc.right()) break;
        else if(rcItem.bottom() < rc.bottom())
        {
            painter.drawText(rcItem, Qt::TextSingleLine, textItem);
        }
        else
        {
            cy = rcStatus.bottom();
            cx = rcItem.right() + 10;

            rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, textItem);
            if(rcItem.right() > rc.right()) break;

            painter.drawText(rcItem, Qt::TextSingleLine, textItem);
        }

        cy += rcItem.height();
    }
}

///
/// \brief OutputWidget::updateTraffic
/// \param request
/// \param server
///
void OutputWidget::updateTraffic(const QModbusRequest& request, int server)
{
    updateTrafficWidget(true, server, request);
}

///
/// \brief OutputWidget::updateTraffic
/// \param response
/// \param server
///
void OutputWidget::updateTraffic(const QModbusResponse& response, int server)
{
    updateTrafficWidget(false, server, response);
}

///
/// \brief OutputWidget::updateData
///
void OutputWidget::updateData(const QModbusDataUnit& data)
{
    updateDataWidget(data);
    _lastData = data;
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
    updateDataWidget(_lastData);
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
            outValue = value1;
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const float value = makeFloat(value1, value2);
            outValue = value;
            result = QLocale().toString(value);
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
            outValue = value1;
            result = QString("<%1>").arg(value1);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const double value = makeDouble(value1, value2, value3, value4);
            outValue = value;
            result = QLocale().toString(value);
        }
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatAddress
/// \param pointType
/// \param address
/// \param hexFormat
/// \return
///
QString formatAddress(QModbusDataUnit::RegisterType pointType, int address, bool hexFormat)
{
    QString prefix;
    switch(pointType)
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

    return hexFormat ? QStringLiteral("%1H").arg(address, 4, 16, QLatin1Char('0')) :
               prefix + QStringLiteral("%1").arg(address, 4, 10, QLatin1Char('0'));
}

///
/// \brief OutputWidget::on_listWidget_itemDoubleClicked
/// \param item
///
void OutputWidget::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item == nullptr) return;
    auto itemData = item->data(Qt::UserRole).value<ListItemData>();

    switch(_displayDefinition.PointType)
    {
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            switch(_dataDisplayMode)
            {
                case DataDisplayMode::FloatingPt:
                case DataDisplayMode::SwappedFP:
                    if(itemData.Row % 2)
                    {
                        const auto item = ui->listWidget->item(itemData.Row - 1);
                        if(item) itemData = item->data(Qt::UserRole).value<ListItemData>();
                    }
                break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
                    if(itemData.Row % 4)
                    {
                        const auto item = ui->listWidget->item(itemData.Row - itemData.Row % 4);
                        if(item) itemData = item->data(Qt::UserRole).value<ListItemData>();
                    }
                break;

                default:
                break;
            }
        }
        break;

        default:
        break;
    }

    emit itemDoubleClicked(itemData.Address, itemData.Value);
}

///
/// \brief OutputWidget::captureString
/// \param s
///
void OutputWidget::captureString(const QString& s)
{
    if(_fileCapture.isOpen())
    {
       QTextStream stream(&_fileCapture);
       stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << " " <<
              formatAddress(_displayDefinition.PointType, _displayDefinition.PointAddress, false) << " "
              << s << "\n";
    }
}

///
/// \brief OutputWidget::updateDataWidget
/// \param data
///
void OutputWidget::updateDataWidget(const QModbusDataUnit& data)
{
    if(!data.isValid() || !_lastData.isValid() ||
       data.valueCount() != _lastData.valueCount() ||
       data.startAddress() != _lastData.startAddress() ||
       data.registerType() != _lastData.registerType())
    {
        ui->listWidget->clear();
    }

    QStringList capstr;
    for(int i = 0; i < _displayDefinition.Length; i++)
    {
        ListItemData itemData;
        itemData.Row = i;
        itemData.Address = i + _displayDefinition.PointAddress;

        const auto addr = formatAddress(_displayDefinition.PointType, itemData.Address, _displayHexAddreses);
        const auto value = data.value(i);
        const auto format = "%1: %2                ";

        QString valstr;
        switch(_dataDisplayMode)
        {
            case DataDisplayMode::Binary:
                valstr = formatBinatyValue(_displayDefinition.PointType, value, itemData.Value);
            break;

            case DataDisplayMode::Decimal:
                valstr = formatDecimalValue(_displayDefinition.PointType, value, itemData.Value);
            break;

            case DataDisplayMode::Integer:
                valstr = formatIntegerValue(_displayDefinition.PointType, value, itemData.Value);
            break;

            case DataDisplayMode::Hex:
                valstr = formatHexValue(_displayDefinition.PointType, value, itemData.Value);
            break;

            case DataDisplayMode::FloatingPt:
                valstr = formatFloatValue(_displayDefinition.PointType, value, data.value(i + 1),
                                          (i%2) || (i+1>=_displayDefinition.Length), itemData.Value);
            break;

            case DataDisplayMode::SwappedFP:
                valstr = formatFloatValue(_displayDefinition.PointType, data.value(i + 1), value,
                                          (i%2) || (i+1>=_displayDefinition.Length), itemData.Value);
            break;

            case DataDisplayMode::DblFloat:
                valstr = formatDoubleValue(_displayDefinition.PointType, value, data.value(i + 1), data.value(i + 2), data.value(i + 3),
                                           (i%4) || (i+3>=_displayDefinition.Length), itemData.Value);
            break;

            case DataDisplayMode::SwappedDbl:
                valstr = formatDoubleValue(_displayDefinition.PointType, data.value(i + 3), data.value(i + 2), data.value(i + 1), value,
                                           (i%4) || (i+3>=_displayDefinition.Length), itemData.Value);
            break;
        }

        if(captureMode() == CaptureMode::TextCapture)
            capstr.push_back(QString(valstr).remove('<').remove('>'));

        auto item = ui->listWidget->item(i);
        if(!item) {
            item = new QListWidgetItem(ui->listWidget);
            ui->listWidget->addItem(item);
        }
        item->setText(QString(format).arg(addr, valstr));
        item->setData(Qt::UserRole, QVariant::fromValue(itemData));
    }

    if(captureMode() == CaptureMode::TextCapture)
        captureString(capstr.join(' '));
}

///
/// \brief OutputWidget::updateTrafficWidget
/// \param request
/// \param pdu
///
void OutputWidget::updateTrafficWidget(bool request, int server, const QModbusPdu& pdu)
{
    QByteArray rawData;
    rawData.push_back(server);
    rawData.push_back(pdu.functionCode() | ( pdu.isException() ? QModbusPdu::ExceptionByte : 0));
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
    fmt.setForeground(request? foregroundColor() : Qt::white);
    fmt.setBackground(request? Qt::transparent : Qt::black);
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);

    if(request && ui->plainTextEdit->toPlainText().length() > 22000)
        ui->plainTextEdit->clear();

    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}
