#include <QDateTime>
#include <QPainter>
#include <QTextStream>
#include <QInputDialog>
#include "formatutils.h"
#include "htmldelegate.h"
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
/// \brief OutputListModel::OutputListModel
/// \param parent
///
OutputListModel::OutputListModel(OutputWidget* parent)
    : QAbstractListModel(parent)
    ,_parentWidget(parent)
    ,_iconPointGreen(QIcon(":/res/pointGreen.png"))
    ,_iconPointEmpty(QIcon(":/res/pointEmpty.png"))
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
    const auto hexAddresses = _parentWidget->displayHexAddresses();

    const ItemData& itemData = _mapItems[row];
    const auto addrstr = formatAddress(pointType, itemData.Address, hexAddresses);

    switch(role)
    {
        case Qt::DisplayRole:
        {
            auto str = QString("%1: %2").arg(addrstr, itemData.ValueStr);
            const int length = str.length();
            const auto descr = itemData.Description.length() > 20 ?
                        QString("%1...").arg(itemData.Description.left(18)): itemData.Description;
            if(!descr.isEmpty()) str += QString("; %1").arg(descr);
            return str.leftJustified(length + 16, ' ');
        }

        case CaptureRole:
            return QString(itemData.ValueStr).remove('<').remove('>');

        case AddressStringRole:
            return addrstr;

        case AddressRole:
            return itemData.Address;

        case ValueRole:
            return itemData.Value;

        case DescriptionRole:
            return itemData.Description;

        case Qt::DecorationRole:
            return itemData.Simulated ? _iconPointGreen : _iconPointEmpty;
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


        case DescriptionRole:
            _mapItems[index.row()].Description = value.toString();
            emit dataChanged(index, index, QVector<int>() << role);
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
    const auto pointType = _parentWidget->_displayDefinition.PointType;
    const auto byteOrder = _parentWidget->byteOrder();

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

            case DataDisplayMode::Decimal:
                itemData.ValueStr = formatDecimalValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Integer:
                itemData.ValueStr = formatIntegerValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::Hex:
                itemData.ValueStr = formatHexValue(pointType, value, byteOrder, itemData.Value);
            break;

            case DataDisplayMode::FloatingPt:
                itemData.ValueStr = formatFloatValue(pointType, value, _lastData.value(i+1), byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedFP:
                itemData.ValueStr = formatFloatValue(pointType, _lastData.value(i+1), value, byteOrder,
                                          (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::DblFloat:
                itemData.ValueStr = formatDoubleValue(pointType, value, _lastData.value(i+1), _lastData.value(i+2), _lastData.value(i+3),
                                           byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedDbl:
                itemData.ValueStr = formatDoubleValue(pointType, _lastData.value(i+3), _lastData.value(i+2), _lastData.value(i+1), value,
                                           byteOrder, (i%4) || (i+3>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::LongInteger:
                itemData.ValueStr = formatLongValue(pointType, value, _lastData.value(i+1), byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedLI:
                itemData.ValueStr = formatLongValue(pointType, _lastData.value(i+1), value, byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);

            break;

            case DataDisplayMode::UnsignedLongInteger:
                itemData.ValueStr = formatUnsignedLongValue(pointType, value, _lastData.value(i+1), byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;

            case DataDisplayMode::SwappedUnsignedLI:
                itemData.ValueStr = formatUnsignedLongValue(pointType, _lastData.value(i+1), value, byteOrder,
                                              (i%2) || (i+1>=rowCount()), itemData.Value);
            break;
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << Qt::DisplayRole);
}

///
/// \brief OutputListModel::find
/// \param type
/// \param addr
/// \return
///
QModelIndex OutputListModel::find(QModbusDataUnit::RegisterType type, quint16 addr) const
{
    if(_parentWidget->_displayDefinition.PointType != type)
        return QModelIndex();

    const int row = addr - _parentWidget->_displayDefinition.PointAddress;
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
   ,_byteOrder(ByteOrder::LittleEndian)
   ,_listModel(new OutputListModel(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listView->setModel(_listModel.get());
    ui->labelStatus->setAutoFillBackground(true);

    setAutoFillBackground(true);
    setForegroundColor(Qt::black);
    setBackgroundColor(Qt::lightGray);

    setStatusColor(Qt::red);
    setUninitializedStatus();

    connect(ui->logView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, [&](const QItemSelection& sel) {
                if(sel.indexes().isEmpty())
                    ui->modbusMsg->clear();
                else
                    showModbusMessage(sel.indexes().first());
            });
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
/// \param simulations
///
void OutputWidget::setup(const DisplayDefinition& dd, const ModbusSimulationMap& simulations)
{
    _descriptionMap.insert(descriptionMap());
    _displayDefinition = dd;

    _listModel->clear();

    for(auto&& key : simulations.keys())
        _listModel->setData(_listModel->find(key.first, key.second), true, SimulationRole);

    for(auto&& key : _descriptionMap.keys())
        setDescription(key.first, key.second, _descriptionMap[key]);

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
}

///
/// \brief OutputWidget::font
/// \return
///
QFont OutputWidget::font() const
{
    return ui->listView->font();
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
    for(int i = 0; i < _listModel->rowCount(); ++i)
    {
        const auto text = _listModel->data(_listModel->index(i), Qt::DisplayRole).toString().trimmed();
        auto rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);

        if(rcItem.right() > rc.right()) break;
        else if(rcItem.bottom() < rc.bottom())
        {
            painter.drawText(rcItem, Qt::TextSingleLine, text);
        }
        else
        {
            cy = rcStatus.bottom();
            cx = rcItem.right() + 10;

            rcItem = painter.boundingRect(cx, cy, rc.width() - cx, rc.height() - cy, Qt::TextSingleLine, text);
            if(rcItem.right() > rc.right()) break;

            painter.drawText(rcItem, Qt::TextSingleLine, text);
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
    updateLogView(true, server, request);
}

///
/// \brief OutputWidget::updateTraffic
/// \param response
/// \param server
///
void OutputWidget::updateTraffic(const QModbusResponse& response, int server)
{
    updateLogView(false, server, response);
}

///
/// \brief OutputWidget::updateData
///
void OutputWidget::updateData(const QModbusDataUnit& data)
{
    _listModel->updateData(data);

    if(captureMode() == CaptureMode::TextCapture)
    {
        QStringList capstr;
        for(int i = 0; i < _listModel->rowCount(); i++)
            capstr.push_back(_listModel->data(_listModel->index(i), CaptureRole).toString());
        captureString(capstr.join(' '));
    }
}

///
/// \brief OutputWidget::descriptionMap
/// \return
///
AddressDescriptionMap OutputWidget::descriptionMap() const
{
    AddressDescriptionMap descriptionMap;
    for(int i = 0; i < _listModel->rowCount(); i++)
    {
        const auto desc = _listModel->data(_listModel->index(i), DescriptionRole).toString();
        const quint16 addr = _listModel->data(_listModel->index(i), AddressRole).toUInt();
        descriptionMap[{_displayDefinition.PointType, addr}] = desc;
    }
    return descriptionMap;
}

///
/// \brief OutputWidget::setDescription
/// \param type
/// \param addr
/// \param desc
///
void OutputWidget::setDescription(QModbusDataUnit::RegisterType type, quint16 addr, const QString& desc)
{
    _listModel->setData(_listModel->find(type, addr), desc, DescriptionRole);
}

///
/// \brief OutputWidget::setSimulated
/// \param type
/// \param addr
/// \param on
///
void OutputWidget::setSimulated(QModbusDataUnit::RegisterType type, quint16 addr, bool on)
{
    _listModel->setData(_listModel->find(type, addr), on, SimulationRole);
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
    _listModel->update();
}

///
/// \brief OutputWidget::on_listView_customContextMenuRequested
/// \param pos
///
void OutputWidget::on_listView_customContextMenuRequested(const QPoint &pos)
{
    const auto index = ui->listView->indexAt(pos);
    if(!index.isValid()) return;

    QInputDialog dlg(this);
    dlg.setLabelText(QString(tr("%1: Enter Description")).arg(_listModel->data(index, AddressStringRole).toString()));
    dlg.setTextValue(_listModel->data(index, DescriptionRole).toString());
    if(dlg.exec() == QDialog::Accepted)
    {
        _listModel->setData(index, dlg.textValue(), DescriptionRole);
    }
}

///
/// \brief OutputWidget::on_listView_doubleClicked
/// \param index
///
void OutputWidget::on_listView_doubleClicked(const QModelIndex& index)
{
    if(!index.isValid()) return;

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
                case DataDisplayMode::LongInteger:
                case DataDisplayMode::SwappedLI:
                case DataDisplayMode::UnsignedLongInteger:
                case DataDisplayMode::SwappedUnsignedLI:
                    if(index.row() % 2)
                        idx = _listModel->index(index.row() - 1);
                break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
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

    const auto address = _listModel->data(idx, AddressRole).toUInt();
    const auto value = _listModel->data(idx, ValueRole);

    emit itemDoubleClicked(address, value);
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
       stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << " " <<
              formatAddress(_displayDefinition.PointType, _displayDefinition.PointAddress, false) << " "
              << s << "\n";
    }
}

///
/// \brief OutputWidget::showModbusMessage
/// \param index
///
void OutputWidget::showModbusMessage(const QModelIndex& index)
{
    const auto msg = ui->logView->itemAt(index);
    ui->modbusMsg->setModbusMessage(msg);
}

///
/// \brief OutputWidget::updateLogView
/// \param request
/// \param pdu
///
void OutputWidget::updateLogView(bool request, int server, const QModbusPdu& pdu)
{
    ui->logView->addItem(pdu, QDateTime::currentDateTime(), server, request);
}
