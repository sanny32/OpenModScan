#include <QMenu>
#include <QWheelEvent>
#include <QDateTime>
#include <QPainter>
#include <QTextStream>
#include <QTextDocument>
#include <QInputDialog>
#include "fontutils.h"
#include "htmldelegate.h"
#include "formatutils.h"
#include "outputwidget.h"
#include "modbusmessages.h"
#include "ui_outputwidget.h"

///
/// \brief SimulationRole
///
const int SimulationRole = Qt::UserRole + 1;

///
/// \brief CaptureRole
///
const int CaptureRole = Qt::UserRole + 2;

///
/// \brief DescriptionRole
///
const int DescriptionRole = Qt::UserRole + 3;

///
/// \brief AddressStringRole
///
const int AddressStringRole = Qt::UserRole + 4;

///
/// \brief AddressRole
///
const int AddressRole = Qt::UserRole + 5;

///
/// \brief ValueRole
///
const int ValueRole = Qt::UserRole + 6;

///
/// \brief ColorRole
///
const int ColorRole = Qt::UserRole + 7;

///
/// \brief htmlPad
/// \param count
/// \return
///
QString htmlPad(int count)
{
    return QString("&nbsp;").repeated(count);
}

///
/// \brief normalizeHtml
/// \param s
/// \return
///
QString normalizeHtml(const QString& s)
{
    QString out = s;
    out.replace("&", "&amp;");
    out.replace("<", "&lt;");
    out.replace(">", "&gt;");
    out.replace("\"", "&quot;");
    out.replace("'", "&#39;");
    out.replace(" ", "&nbsp;");
    return out;
}

///
/// \brief OutputListModel::OutputListModel
/// \param parent
///
OutputListModel::OutputListModel(OutputWidget* parent)
    : QAbstractListModel(parent)
    ,_parentWidget(parent)
    ,_iconSimulation16Bit(QPixmap(":/res/iconSimulation16bit.png"))
    ,_iconSimulation32Bit(QPixmap(":/res/iconSimulation32bit.png"))
    ,_iconSimulation64Bit(QPixmap(":/res/iconSimulation64bit.png"))
    ,_iconSimulationOff(_iconSimulation16Bit.size())
{
}

///
/// \brief OutputListModel::rowCount
/// \return
///
int OutputListModel::rowCount(const QModelIndex&) const
{
    return _parentWidget->_displayDefinition.Length;
}

///
/// \brief OutputListModel::data
/// \param index
/// \param role
/// \return
///
QVariant OutputListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() ||
       !_mapItems.contains(index.row()))
    {
        return QVariant();
    }

    const auto row = index.row();
    const auto pointType = _parentWidget->_displayDefinition.PointType;
    const auto addrSpace = _parentWidget->_displayDefinition.AddrSpace;
    const auto hexAddresses = _parentWidget->displayHexAddresses();

    const ItemData& itemData = _mapItems[row];
    const auto addrStr = formatAddress(pointType, itemData.Address, addrSpace, hexAddresses);

    switch(role)
    {
        case Qt::DisplayRole:
        {
            const auto fg = itemData.FgColor.isValid() ? itemData.FgColor : _parentWidget->foregroundColor();
            const auto bg = itemData.BgColor.isValid() ? itemData.BgColor : _parentWidget->backgroundColor();

            const QString base = QString("%1: %2").arg(addrStr, itemData.ValueStr);
            const int targetLen = base.length() + _columnsDistance;

            QString descr = itemData.Description;
            if (!descr.isEmpty())
            {
                const QString prefix = "; ";
                const int freeSpace = targetLen - (base.length() + prefix.length());

                if (freeSpace > 0)
                {
                    if (descr.length() > freeSpace)
                        descr = descr.left(freeSpace - 4) + "...";

                    descr = prefix + descr;
                }
            }

            const int pad = targetLen - (base.length() + descr.length());
            return QString(
                       "<span style=\"background-color:%1; color:%2\">%3</span><span>%4%5</span>"
                       ).arg(bg.name(), fg.name(), normalizeHtml(base), normalizeHtml(descr), (pad > 0) ? htmlPad(pad) : "");
        }

        case CaptureRole:
            return QString(itemData.ValueStr).remove('<').remove('>');

        case AddressStringRole:
            return addrStr;

        case AddressRole:
            return itemData.Address;

        case ValueRole:
            return itemData.Value;

        case DescriptionRole:
            return itemData.Description;

        case ColorRole:
            return itemData.BgColor;

        case Qt::DecorationRole:
        {
            if(itemData.ValueStr.isEmpty())
                return _iconSimulationOff;

            switch(simulationIcon(row))
            {
            case SimulationIcon16Bit:
                return _iconSimulation16Bit;
            case SimulationIcon32Bit:
                return _iconSimulation32Bit;
            case SimulationIcon64Bit:
                return _iconSimulation64Bit;

            default:
                return _iconSimulationOff;
            }
        }
    }

    return QVariant();
}

///
/// \brief OutputListModel::setData
/// \param index
/// \param value
/// \param role
/// \return
///
bool OutputListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() ||
       !_mapItems.contains(index.row()))
    {
        return false;
    }

    switch (role)
    {
        case SimulationRole:
            _mapItems[index.row()].Simulated = value.toBool();
            emit dataChanged(index, index, QVector<int>() << role);
        return true;

        case Qt::DecorationRole:
            _mapItems[index.row()].SimulationIcon = value.value<SimulationIconType>();
            emit dataChanged(index, index, QVector<int>() << role);
        return true;

        case DescriptionRole:
            _mapItems[index.row()].Description = value.toString();
            emit dataChanged(index, index, QVector<int>() << role);
        return true;

        case ColorRole:
            _mapItems[index.row()].BgColor = value.value<QColor>();
            emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
        return true;

        default:
        return false;
    }
}

///
/// \brief OutputListModel::isUpdated
/// \return
///
bool OutputListModel::isValid() const
{
    return _lastData.isValid();
}

///
/// \brief OutputListModel::values
/// \return
///
QVector<quint16> OutputListModel::values() const
{
    return _lastData.values();
}

///
/// \brief OutputListModel::clear
///
void OutputListModel::clear()
{
    _mapItems.clear();
    updateData(QModbusDataUnit());
}

///
/// \brief OutputListModel::update
///
void OutputListModel::update()
{
    updateData(_lastData);
}

///
/// \brief OutputListModel::updateData
/// \param data
///
void OutputListModel::updateData(const QModbusDataUnit& data)
{
    _lastData = data;

    const auto mode = _parentWidget->dataDisplayMode();
    const auto leadingZeros = _parentWidget->_displayDefinition.LeadingZeros;
    const auto pointType = _parentWidget->_displayDefinition.PointType;
    const auto byteOrder = _parentWidget->byteOrder();
    const auto codepage = _parentWidget->codepage();

    for(int i = 0; i < rowCount(); i++)
    {
        const auto value = _lastData.value(i);

        auto& itemData = _mapItems[i];
        itemData.Address = _parentWidget->_displayDefinition.PointAddress + i;

        switch(mode)
        {
            case DataDisplayMode::Binary:
                itemData.ValueStr = formatBinaryValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::UInt16:
                itemData.ValueStr = formatUInt16Value(pointType, value, byteOrder, leadingZeros, itemData.Value);
            break;

            case DataDisplayMode::Int16:
                itemData.ValueStr = formatInt16Value(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Hex:
                itemData.ValueStr = formatHexValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Ansi:
                itemData.ValueStr = formatAnsiValue(pointType, value, byteOrder, codepage, itemData.Value);
            break;

            case DataDisplayMode::FloatingPt:
                // MSRF
                itemData.ValueStr = formatFloatValue(pointType, _lastData.value(i+1), value, byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedFP:
                // LSRF
                itemData.ValueStr = formatFloatValue(pointType, value, _lastData.value(i+1), byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::DblFloat:
                // MSRF
                itemData.ValueStr = formatDoubleValue(pointType, _lastData.value(i+3), _lastData.value(i+2), _lastData.value(i+1), value,
                                                      byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedDbl:
                // LSRF
                itemData.ValueStr = formatDoubleValue(pointType, value, _lastData.value(i+1), _lastData.value(i+2), _lastData.value(i+3),
                                                      byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::Int32:
                // MSRF
                itemData.ValueStr = formatInt32Value(pointType, _lastData.value(i+1), value, byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedInt32:
                // LSRF
                itemData.ValueStr = formatInt32Value(pointType, value, _lastData.value(i+1), byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::UInt32:
                // MSRF
                itemData.ValueStr = formatUInt32Value(pointType, _lastData.value(i+1), value, byteOrder, leadingZeros,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedUInt32:
                // LSRF
                itemData.ValueStr = formatUInt32Value(pointType, value, _lastData.value(i+1), byteOrder, leadingZeros,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::Int64:
                // MSRF
                itemData.ValueStr = formatInt64Value(pointType, _lastData.value(i+3), _lastData.value(i+2), _lastData.value(i+1), value,
                                                     byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedInt64:
                 // LSRF
                itemData.ValueStr = formatInt64Value(pointType, value, _lastData.value(i+1), _lastData.value(i+2), _lastData.value(i+3),
                                                     byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::UInt64:
                // MSRF
                itemData.ValueStr = formatUInt64Value(pointType, _lastData.value(i+3), _lastData.value(i+2), _lastData.value(i+1), value,
                                                      byteOrder, leadingZeros, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedUInt64:
                // LSRF
                itemData.ValueStr = formatUInt64Value(pointType, value, _lastData.value(i+1), _lastData.value(i+2), _lastData.value(i+3),
                                                      byteOrder, leadingZeros, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << Qt::DisplayRole);
}

///
/// \brief OutputListModel::simulationIcon
/// \param row
/// \return
///
OutputListModel::SimulationIconType OutputListModel::simulationIcon(int row) const
{
    const auto mode = _parentWidget->dataDisplayMode();
    for(int i = 0; i < registersCount(mode); ++i)
    {
        if(row + i >= rowCount())
            return SimulationIconNone;

        if(_mapItems[row + i].Simulated)
            return _mapItems[row + i].SimulationIcon;
    }

    return SimulationIconNone;
}

///
/// \brief OutputListModel::find
/// \param deviceId
/// \param type
/// \param addr
/// \return
///
QModelIndex OutputListModel::find(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr) const
{
    if(_parentWidget->_displayDefinition.PointType != type || _parentWidget->_displayDefinition.DeviceId != deviceId)
        return QModelIndex();

    const auto dd =  _parentWidget->_displayDefinition;
    const int row = addr - (dd.PointAddress - (dd.ZeroBasedAddress ? 0 : 1));
    if(row >= 0 && row < rowCount())
        return index(row);

    return QModelIndex();
}

///
/// \brief OutputWidget::OutputWidget
/// \param parent
///
OutputWidget::OutputWidget(QWidget *parent) :
     QWidget(parent)
   , ui(new Ui::OutputWidget)
   ,_displayHexAddresses(false)
   ,_displayMode(DisplayMode::Data)
   ,_dataDisplayMode(DataDisplayMode::Binary)
   ,_byteOrder(ByteOrder::Direct)
   ,_listModel(new OutputListModel(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listView->setModel(_listModel.get());
    ui->listView->setItemDelegate(new HtmlDelegate(this));
    ui->labelStatus->setAutoFillBackground(true);

    setFont(defaultMonospaceFont());
    setAutoFillBackground(true);
    setForegroundColor(Qt::black);
    setBackgroundColor(Qt::white);

    setStatusColor(Qt::red);
    setUninitializedStatus();

    hideModbusMessage();

    connect(ui->logView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection& sel) {
                if(!sel.indexes().isEmpty()) {
                    showModbusMessage(sel.indexes().first());
                }
            });

    _baseFontSize = ui->listView->font().pointSizeF();
    if (_baseFontSize <= 0) _baseFontSize = 10.0;

    _zoomLabel = new QLabel(this);
    _zoomLabel->setVisible(false);
    _zoomLabel->setAlignment(Qt::AlignCenter);
    _zoomLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    _zoomLabel->setStyleSheet(R"(
        QLabel {
            background: rgba(30, 30, 30, 180);
            color: white;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 12pt;
        }
    )");

    _zoomHideTimer = new QTimer(this);
    _zoomHideTimer->setSingleShot(true);
    connect(_zoomHideTimer, &QTimer::timeout, _zoomLabel, &QLabel::hide);

    ui->listView->viewport()->installEventFilter(this);
}

///
/// \brief OutputWidget::~OutputWidget
///
OutputWidget::~OutputWidget()
{
    delete ui;
}

///
/// \brief OutputWidget::changeEvent
/// \param event
///
void OutputWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        if(!_listModel->isValid())
            setUninitializedStatus();
    }

    QWidget::changeEvent(event);
}

///
/// \brief OutputWidget::eventFilter
/// \param obj
/// \param event
/// \return
///
bool OutputWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->listView->viewport() &&
        event->type() == QEvent::Wheel)
    {
        auto we = static_cast<QWheelEvent*>(event);
        if (we->modifiers() & Qt::ControlModifier)
        {
            if (we->angleDelta().y() > 0)
                setZoomPercent(zoomPercent() + 10);
            else if (we->angleDelta().y() < 0)
                setZoomPercent(zoomPercent() - 10);

            showZoomOverlay();

            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

///
/// \brief OutputWidget::showZoomOverlay
/// \param currentFontSize
///
void OutputWidget::showZoomOverlay()
{
    _zoomLabel->setText(tr("Zoom: %1%").arg(_zoomPercent));
    _zoomLabel->adjustSize();

    if(!_zoomLabel->isVisible())
    {
        const QPoint centerInThis = ui->listView->viewport()->mapTo(this, ui->listView->viewport()->rect().center());
        const QPoint center = centerInThis - QPoint(_zoomLabel->width() / 2, _zoomLabel->height() / 2);

        _zoomLabel->move(center);
        _zoomLabel->show();
        _zoomLabel->raise();
    }

    _zoomHideTimer->start(800);
}

///
/// \brief OutputWidget::data
/// \return
///
QVector<quint16> OutputWidget::data() const
{
    return _listModel->values();
}

///
/// \brief OutputWidget::setup
/// \param dd
/// \param protocol
/// \param simulations
///
void OutputWidget::setup(const DisplayDefinition& dd, ModbusMessage::ProtocolType protocol, const ModbusSimulationMap2& simulations)
{
    _descriptionMap.insert(descriptionMap());
    _colorMap.insert(colorMap());

    _displayDefinition = dd;

    setProtocol(protocol);
    setLogViewLimit(dd.LogViewLimit);
    setDataViewColumnsDistance(dd.DataViewColumnsDistance);
    setAutosctollLogView(dd.AutoscrollLog);
    ui->logView->setShowLeadingZeros(dd.LeadingZeros);
    ui->modbusMsg->setShowLeadingZeros(dd.LeadingZeros);

    _listModel->clear();

    for(auto&& key : simulations.keys())
        _listModel->setData(_listModel->find(key.DeviceId, key.Type, key.Address), true, SimulationRole);

    for(auto&& key : _descriptionMap.keys())
        setDescription(key.DeviceId, key.Type, key.Address, _descriptionMap[key]);

    for(auto&& key : _colorMap.keys())
        setColor(key.DeviceId, key.Type, key.Address, _colorMap[key]);

    _listModel->update();

}

///
/// \brief OutputWidget::displayHexAddresses
/// \return
///
bool OutputWidget::displayHexAddresses() const
{
    return _displayHexAddresses;
}

///
/// \brief OutputWidget::setDisplayHexAddresses
/// \param on
///
void OutputWidget::setDisplayHexAddresses(bool on)
{
    _displayHexAddresses = on;
    _listModel->update();
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
    if(!_fileCapture.open(QFile::Text | QFile::WriteOnly))
        emit startTextCaptureError(_fileCapture.errorString());
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
    return ui->listView->palette().color(QPalette::Base);
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
    return ui->listView->palette().color(QPalette::Text);
}

///
/// \brief OutputWidget::setForegroundColor
/// \param clr
///
void OutputWidget::setForegroundColor(const QColor& clr)
{
    auto pal = ui->listView->palette();
    pal.setColor(QPalette::Text, clr);
    ui->listView->setPalette(pal);
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
    ui->modbusMsg->setStatusColor(clr);
}

///
/// \brief OutputWidget::font
/// \return
///
QFont OutputWidget::font() const
{
    return ui->logView->font();
}

///
/// \brief OutputWidget::setFont
/// \param font
///
void OutputWidget::setFont(const QFont& font)
{
    ui->listView->setFont(font);
    ui->labelStatus->setFont(font);
    ui->logView->setFont(font);
    ui->modbusMsg->setFont(font);

    _zoomPercent = 100;
    _baseFontSize = ui->listView->font().pointSizeF();
    if (_baseFontSize <= 0) _baseFontSize = 10.0;
}

///
/// \brief OutputWidget::zoomPercent
/// \return
///
int OutputWidget::zoomPercent() const
{
    return _zoomPercent;
}

///
/// \brief OutputWidget::setZoomPercent
/// \param zoom
///
void OutputWidget::setZoomPercent(int zoomPercent)
{
    _zoomPercent = qBound(50, zoomPercent, 300);

    QFont font = ui->listView->font();
    font.setPointSizeF(_baseFontSize * _zoomPercent / 100.0);

    ui->listView->setFont(font);
    ui->labelStatus->setFont(font);
}

///
/// \brief OutputWidget::dataViewColumnsDistance
/// \return
///
int OutputWidget::dataViewColumnsDistance() const
{
    return _listModel->columnsDistance();
}

///
/// \brief OutputWidget::setDataViewColumnsDistance
/// \param value
///
void OutputWidget::setDataViewColumnsDistance(int value)
{
    _listModel->setColumnsDistance(value);
}

///
/// \brief OutputWidget::logViewLimit
/// \return
///
int OutputWidget::logViewLimit() const
{
    return ui->logView->rowLimit();
}

///
/// \brief OutputWidget::setLogViewLimit
/// \param l
///
void OutputWidget::setLogViewLimit(int l)
{
    ui->logView->setRowLimit(l);
}

///
/// \brief OutputWidget::autoscrollLogView
/// \return
///
bool OutputWidget::autoscrollLogView() const
{
    return ui->logView->autoscroll();
}

///
/// \brief OutputWidget::setAutosctollLogView
/// \param on
///
void OutputWidget::setAutosctollLogView(bool on)
{
    ui->logView->setAutoscroll(on);
}

///
/// \brief OutputWidget::clearLogView
///
void OutputWidget::clearLogView()
{
    ui->logView->clear();
    ui->modbusMsg->clear();
    hideModbusMessage();
}

///
/// \brief OutputWidget::setStatus
/// \param status
///
void OutputWidget::setStatus(const QString& status)
{
    if(status.isEmpty()) {
        ui->labelStatus->setText(status);
    }
    else {
        const auto info = QString("*** %1 ***").arg(status);
        if(info != ui->labelStatus->text())
            ui->labelStatus->setText(info);
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
    int maxWidth = 0;
    for(int i = 0; i < _listModel->rowCount(); ++i)
    {
        QTextDocument doc;
        doc.setHtml(_listModel->data(_listModel->index(i), Qt::DisplayRole).toString());
        const auto text = doc.toPlainText().trimmed();

        auto rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);
        maxWidth = qMax(maxWidth, rcItem.width());

        if(rcItem.right() > rc.right()) break;
        else if(rcItem.bottom() < rc.bottom())
        {
            painter.drawText(rcItem, Qt::TextSingleLine, text);
        }
        else
        {
            cy = rcStatus.bottom();
            cx = rcItem.left() + maxWidth + 10;

            rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);
            if(rcItem.right() > rc.right()) break;

            painter.drawText(rcItem, Qt::TextSingleLine, text);
        }

        cy += rcItem.height();
    }
}

///
/// \brief OutputWidget::updateTraffic
/// \param msg
///
void OutputWidget::updateTraffic(QSharedPointer<const ModbusMessage> msg)
{
    updateLogView(msg);
}

///
/// \brief OutputWidget::updateData
///
void OutputWidget::updateData(const QModbusDataUnit& data)
{
    _listModel->updateData(data);
}

///
/// \brief OutputWidget::colorMap
/// \return
///
AddressColorMap2 OutputWidget::colorMap() const
{
    AddressColorMap2 colorMap;
    for(int i = 0; i < _listModel->rowCount(); i++)
    {
        const auto clr = _listModel->data(_listModel->index(i), ColorRole).value<QColor>();
        const quint16 addr = _listModel->data(_listModel->index(i), AddressRole).toUInt() - (_displayDefinition.ZeroBasedAddress ? 0 : 1);
        colorMap[{_displayDefinition.DeviceId, _displayDefinition.PointType, addr }] = clr;
    }
    return colorMap;
}

///
/// \brief OutputWidget::setColor
/// \param type
/// \param addr
/// \param clr
///
void OutputWidget::setColor(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const QColor& clr)
{
     _listModel->setData(_listModel->find(deviceId, type, addr), clr, ColorRole);
}

///
/// \brief OutputWidget::descriptionMap
/// \return
///
AddressDescriptionMap2 OutputWidget::descriptionMap() const
{
    AddressDescriptionMap2 descriptionMap;
    for(int i = 0; i < _listModel->rowCount(); i++)
    {
        const auto desc = _listModel->data(_listModel->index(i), DescriptionRole).toString();
        const quint16 addr = _listModel->data(_listModel->index(i), AddressRole).toUInt() - (_displayDefinition.ZeroBasedAddress ? 0 : 1);
        descriptionMap[{_displayDefinition.DeviceId, _displayDefinition.PointType, addr }] = desc;
    }
    return descriptionMap;
}

///
/// \brief OutputWidget::setDescription
/// \param type
/// \param addr
/// \param desc
///
void OutputWidget::setDescription(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const QString& desc)
{
    _listModel->setData(_listModel->find(deviceId, type, addr), desc, DescriptionRole);
}

///
/// \brief OutputWidget::setSimulated
/// \param mode
/// \param deviceId
/// \param type
/// \param addr
/// \param on
///
void OutputWidget::setSimulated(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, bool on)
{
    const auto index = _listModel->find(deviceId, type, addr);
    _listModel->setData(index, on, SimulationRole);

    if(on) {
        switch(registersCount(mode))
        {
        case 1:
            _listModel->setData(index, OutputListModel::SimulationIcon16Bit, Qt::DecorationRole);
            break;
        case 2:
            _listModel->setData(index, OutputListModel::SimulationIcon32Bit, Qt::DecorationRole);
            break;
        case 4:
            _listModel->setData(index, OutputListModel::SimulationIcon64Bit, Qt::DecorationRole);
            break;
        }
    }
    else {
        _listModel->setData(index, OutputListModel::SimulationIconNone, Qt::DecorationRole);
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
    ui->logView->setDataDisplayMode(mode);
    ui->modbusMsg->setDataDisplayMode(mode);

    _listModel->update();
}

///
/// \brief OutputWidget::protocol
/// \return
///
ModbusMessage::ProtocolType OutputWidget::protocol() const
{
    return _protocol;
}

///
/// \brief OutputWidget::setProtocol
/// \param type
///
void OutputWidget::setProtocol(ModbusMessage::ProtocolType type)
{
    _protocol = type;
}

///
/// \brief OutputWidget::byteOrder
/// \return
///
ByteOrder OutputWidget::byteOrder() const
{
    return _byteOrder;
}

///
/// \brief OutputWidget::setByteOrder
/// \param order
///
void OutputWidget::setByteOrder(ByteOrder order)
{
    _byteOrder = order;
    ui->modbusMsg->setByteOrder(order);

    _listModel->update();
}

///
/// \brief OutputWidget::codepage
/// \return
///
QString OutputWidget::codepage() const
{
    return _codepage;
}

///
/// \brief OutputWidget::setCodepage
/// \param name
///
void OutputWidget::setCodepage(const QString& name)
{
    _codepage = name;
    _listModel->update();
}

///
/// \brief drawRemoveColorIcon
/// \param size
/// \return
///
QIcon drawRemoveColorIcon(int size = 16)
{
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QRect r(2, 2, size - 4, size - 4);
    p.setBrush(Qt::white);
    p.setPen(QPen(Qt::black, 0.1));
    p.drawRect(r);

    QPen pen(Qt::red, 1);
    p.setPen(pen);
    p.drawLine(0, size, size, 0);

    return QIcon(pm);
}

///
/// \brief OutputWidget::on_listView_customContextMenuRequested
/// \param pos
///
void OutputWidget::on_listView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->listView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu menu(this);

    const auto idx = getValueIndex(index);
    const auto address = _listModel->data(idx, AddressStringRole).toString();

    QAction* writeValueAction = menu.addAction(tr("Write Value to %1").arg(address));
    writeValueAction->setEnabled(emit canWriteValue());
    connect(writeValueAction, &QAction::triggered, this, [this, index](){
        emit ui->listView->doubleClicked(index);
    });

    const auto desc = _listModel->data(index, DescriptionRole).toString();
    QAction* addDescrAction = menu.addAction(desc.isEmpty() ? tr("Add Description") : tr("Edit Description"));
    connect(addDescrAction, &QAction::triggered, this, [this, index, address](){
        QInputDialog dlg(this);
        dlg.setLabelText(QString(tr("%1: Enter Description")).arg(_listModel->data(index, AddressStringRole).toString()));
        dlg.setTextValue(_listModel->data(index, DescriptionRole).toString());
        if(dlg.exec() == QDialog::Accepted) {
            _listModel->setData(index, dlg.textValue(), DescriptionRole);
        }
    });

    menu.addSeparator();

    QAction* removeColorAction = menu.addAction(drawRemoveColorIcon(), tr("Remove Color"));
    const auto clr = _listModel->data(index, ColorRole).value<QColor>();
    removeColorAction->setEnabled(clr.isValid());
    connect(removeColorAction, &QAction::triggered, this, [this, index](){
        _listModel->setData(index, QColor(), ColorRole);
    });

    menu.addSeparator();

    struct ColorItem { QString name; QColor color; };
    QVector<ColorItem> safeColors = {
        { tr("Yellow"), QColor(Qt::yellow) },
        { tr("Cyan"), QColor(Qt::cyan) },
        { tr("Magenta"), QColor(Qt::magenta) },
        { tr("LightGreen"), QColor(144, 238, 144) },
        { tr("Orange"), QColor(255, 165, 0) },
        { tr("LightBlue"), QColor(173, 216, 230) },
        { tr("LightGray"), QColor(Qt::lightGray) }
    };

    for (const auto &c : safeColors)
    {
        QPixmap pixmap(16,16);
        pixmap.fill(c.color);
        QIcon icon(pixmap);

        QAction* colorAction = menu.addAction(icon, c.name);
        connect(colorAction, &QAction::triggered, this, [this, index, c](){
            _listModel->setData(index, c.color, ColorRole);
        });
    }

    menu.exec(ui->listView->viewport()->mapToGlobal(pos));
}

///
/// \brief OutputWidget::on_listView_doubleClicked
/// \param index
///
void OutputWidget::on_listView_doubleClicked(const QModelIndex& index)
{
    if(!index.isValid()) return;

    const auto idx = getValueIndex(index);
    const auto address = _listModel->data(idx, AddressRole).toUInt();
    const auto value = _listModel->data(idx, ValueRole);

    emit itemDoubleClicked(address, value);
}

///
/// \brief OutputWidget::getValueIndex
/// \param index
/// \return
///
QModelIndex OutputWidget::getValueIndex(const QModelIndex& index) const
{
    QModelIndex idx = index;
    switch(_displayDefinition.PointType)
    {
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            switch(_dataDisplayMode)
            {
                case DataDisplayMode::FloatingPt:
                case DataDisplayMode::SwappedFP:
                case DataDisplayMode::Int32:
                case DataDisplayMode::SwappedInt32:
                case DataDisplayMode::UInt32:
                case DataDisplayMode::SwappedUInt32:
                    if(index.row() % 2)
                        idx = _listModel->index(index.row() - 1);
                    break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
                case DataDisplayMode::Int64:
                case DataDisplayMode::SwappedInt64:
                case DataDisplayMode::UInt64:
                case DataDisplayMode::SwappedUInt64:
                    if(index.row() % 4)
                        idx = _listModel->index(index.row() - index.row() % 4);
                    break;

                default:
                    break;
            }
        }
        break;

        default:
            break;
    }
    return idx;
}


///
/// \brief OutputWidget::setUninitializedStatus
///
void OutputWidget::setUninitializedStatus()
{
    setStatus(tr("Data Uninitialized"));
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
       stream << s << "\n";
    }
}

///
/// \brief OutputWidget::showModbusMessage
/// \param index
///
void OutputWidget::showModbusMessage(const QModelIndex& index)
{
    const auto msg = ui->logView->itemAt(index);
    if(msg) {
        if(ui->splitter->widget(1)->isHidden()) {
            ui->splitter->setSizes({1, 1});
            ui->splitter->widget(1)->show();
        }
        ui->modbusMsg->setModbusMessage(msg);
    }
}

///
/// \brief OutputWidget::hideModbusMessage
///
void OutputWidget::hideModbusMessage()
{
    ui->splitter->setSizes({1, 0});
    ui->splitter->widget(1)->hide();
}

///
/// \brief OutputWidget::updateLogView
/// \param msg
///
void OutputWidget::updateLogView(QSharedPointer<const ModbusMessage> msg)
{
    ui->logView->addItem(msg);
    if(captureMode() == CaptureMode::TextCapture && msg != nullptr)
    {
        const auto str = QString("%1: %2 %3 %4").arg(
            (msg->isRequest()?  "Tx" : "Rx"),
            msg->timestamp().toString(Qt::ISODateWithMs),
            (msg->isRequest()?  "<<" : ">>"),
            msg->toString(DataDisplayMode::Hex, _displayDefinition.LeadingZeros));
        captureString(str);
    }
}
