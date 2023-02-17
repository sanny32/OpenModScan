#include <QDateTime>
#include <QAbstractEventDispatcher>
#include "modbuslimits.h"
#include "dialogaddressscan.h"
#include "ui_dialogaddressscan.h"

///
/// \brief formatAddress
/// \param pointType
/// \param address
/// \return
///
QString formatAddress(QModbusDataUnit::RegisterType pointType, int address)
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

    return prefix + QStringLiteral("%1").arg(address, 4, 10, QLatin1Char('0'));
}

///
/// \brief TableViewItemModel::TableViewItemModel
/// \param data
/// \param columns
/// \param parent
///
TableViewItemModel::TableViewItemModel(const ModbusDataUnit& data, int columns, QObject* parent)
    : QAbstractTableModel(parent)
    ,_columns(columns)
    ,_data(data)
{
}

///
/// \brief TableViewItemModel::rowCount
/// \return
///
int TableViewItemModel::rowCount(const QModelIndex&) const
{
    return qCeil(_data.valueCount() / (double)_columns);
}

///
/// \brief TableViewItemModel::columnCount
/// \return
///
int TableViewItemModel::columnCount(const QModelIndex&) const
{
    return _columns;
}

///
/// \brief TableViewItemModel::data
/// \param index
/// \param role
/// \return
///
QVariant TableViewItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const auto parentWidget = qobject_cast<QWidget*>(parent());
    const auto idx = index.row() * _columns + index.column();
    switch(role)
    {
        case Qt::ToolTipRole:
            return formatAddress(_data.registerType(), _data.startAddress() + idx);

        case Qt::DisplayRole:
            return _data.hasValue(idx) ? QString::number(_data.value(idx)) : "-";

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::BackgroundRole:
            return _data.hasValue(idx) ? QVariant() : parentWidget->palette().color(QPalette::Disabled, QPalette::Base);

        case Qt::UserRole:
            return _data.startAddress() + idx;
    }

    return QVariant();
}

///
/// \brief TableViewItemModel::setData
/// \param index
/// \param value
/// \param role
/// \return
///
bool TableViewItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::DisplayRole)
    {
        return false;
    }

    const auto idx = index.row() * _columns + index.column();
    _data.setValue(idx, value.toUInt());

    emit dataChanged(index, index, QList<int>() << Qt::DisplayRole);
    return true;
}

///
/// \brief TableViewItemModel::headerData
/// \param section
/// \param orientation
/// \param role
/// \return
///
QVariant TableViewItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0)
    {
        return QVariant();
    }

    switch(role)
    {
        case Qt::DisplayRole:
            switch(orientation)
            {
                case Qt::Horizontal:
                    return QString("+%1").arg(section);

                case Qt::Vertical:
                {
                    const auto length = _data.valueCount();
                    const auto pointAddress = _data.startAddress();
                    const auto addressFrom = QString("%1").arg(pointAddress + section * _columns, 5, 10, QLatin1Char('0'));
                    const auto addressTo = QString("%1").arg(pointAddress + qMin(length - 1, (section + 1) * _columns - 1), 5, 10, QLatin1Char('0'));
                    return QString("%1-%2").arg(addressFrom, addressTo);
                }
            }
        break;
    }

    return QVariant();
}

///
/// \brief TableViewItemModel::flags
/// \param index
/// \return
///
Qt::ItemFlags TableViewItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
       return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///
/// \brief LogViewItemModel::LogViewItemModel
/// \param items
/// \param parent
///
LogViewItemModel::LogViewItemModel(QVector<LogViewItem>& items, QObject* parent)
    : QAbstractListModel(parent)
    ,_items(items)
{
}

///
/// \brief LogViewItemModel::rowCount
/// \param parent
/// \return
///
int LogViewItemModel::rowCount(const QModelIndex&) const
{
    return _items.size();
}

///
/// \brief LogViewItemModel::data
/// \param index
/// \param role
/// \return
///
QVariant LogViewItemModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() ||
       index.row() < 0  ||
       index.row() >= _items.size())
    {
        return QVariant();
    }

    const auto item = _items.at(index.row());
    switch(role)
    {
        case Qt::DisplayRole:
            return item.Text;

        case Qt::BackgroundRole:
            return item.IsRequest ? QVariant() : QColor(Qt::lightGray);

        case Qt::TextAlignmentRole:
            return Qt::AlignVCenter;

        case Qt::UserRole:
            return QVariant::fromValue(item);
    }

    return QVariant();
}

///
/// \brief LogViewItemProxyModel::LogViewItemProxyModel
/// \param parent
///
LogViewItemProxyModel::LogViewItemProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    ,_showValid(false)
{
}

///
/// \brief LogViewItemProxyModel::filterAcceptsRow
/// \param source_row
/// \param source_parent
/// \return
///
bool LogViewItemProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const auto index = sourceModel()->index(source_row, 0, source_parent);
    const auto item = sourceModel()->data(index, Qt::UserRole).value<LogViewItem>();
    return _showValid ? item.IsValid && !item.IsRequest : true;
}

///
/// \brief DialogAddressScan::DialogAddressScan
/// \param dd
/// \param client
/// \param parent
///
DialogAddressScan::DialogAddressScan(const DisplayDefinition& dd, ModbusClient& client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddressScan)
    ,_modbusClient(client)
{
    ui->setupUi(this);
    ui->comboBoxPointType->setCurrentPointType(dd.PointType);
    ui->lineEditStartAddress->setPaddingZeroes(true);
    ui->lineEditStartAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditLength->setInputRange(1, 65530);
    ui->lineEditStartAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(999);
    ui->tabWidget->setCurrentIndex(0);

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogAddressScan::on_awake);

    connect(&_scanTimer, &QTimer::timeout, this, &DialogAddressScan::on_timeout);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &DialogAddressScan::on_modbusReply);
    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &DialogAddressScan::on_modbusRequest);

    clearTableView();
}

///
/// \brief DialogAddressScan::~DialogAddressScan
///
DialogAddressScan::~DialogAddressScan()
{
    delete ui;
}

///
/// \brief DialogAddressScan::on_awake
///
void DialogAddressScan::on_awake()
{
    ui->lineEditStartAddress->setEnabled(!_scanning);
    ui->lineEditLength->setEnabled(!_scanning);
    ui->lineEditSlaveAddress->setEnabled(!_scanning);
    ui->comboBoxPointType->setEnabled(!_scanning);
    ui->progressBar->setVisible(_scanning);
    ui->pushButtonScan->setText(_scanning ? tr("Stop") : tr("Scan"));
}

///
/// \brief DialogAddressScan::on_timeout
///
void DialogAddressScan::on_timeout()
{
    const auto elapsed = QDateTime::fromSecsSinceEpoch(_scanTime++).toUTC().toString("hh:mm:ss");
    ui->labelElapsed->setText(elapsed);
}

///
/// \brief DialogAddressScan::on_lineEditAddress_valueChanged
///
void DialogAddressScan::on_lineEditStartAddress_valueChanged(const QVariant&)
{
}

///
/// \brief DialogAddressScan::on_lineEditLength_valueChanged
///
void DialogAddressScan::on_lineEditLength_valueChanged(const QVariant&)
{
}

///
/// \brief DialogAddressScan::on_lineEditDeviceId_valueChanged
///
void DialogAddressScan::on_lineEditSlaveAddress_valueChanged(const QVariant&)
{

}

///
/// \brief DialogAddressScan::on_comboBoxModbusPointType_pointTypeChanged
///
void DialogAddressScan::on_comboBoxPointType_pointTypeChanged(QModbusDataUnit::RegisterType)
{

}

///
/// \brief DialogAddressScan::on_checkBoxShowValid_toggled
/// \param on
///
void DialogAddressScan::on_checkBoxShowValid_toggled(bool on)
{
    if(!_proxyLogModel)
        return;

    _proxyLogModel->setShowValid(on);
    _logModel->update();
}

///
/// \brief DialogAddressScan::on_modbusRequest
/// \param requestId
/// \param request
///
void DialogAddressScan::on_modbusRequest(int requestId, const QModbusRequest& request)
{
    if(requestId != 0)
    {
        return;
    }

    updateLogView(request);
}

///
/// \brief DialogAddressScan::on_modbusReply
/// \param reply
///
void DialogAddressScan::on_modbusReply(QModbusReply* reply)
{
    if(!_scanning || !reply) return;

    if(0 != reply->property("RequestId").toInt())
    {
        return;
    }

    updateProgress();

    if (reply->error() == QModbusDevice::NoError)
        updateTableView(reply->result().startAddress() + 1, reply->result().value(0));

    updateLogView(reply);

    if(ui->lineEditLength->value<int>() == _requestCount)
        stopScan();
    else
        sendReadRequest();
}

///
/// \brief DialogAddressScan::on_pushButtonScan_clicked
///
void DialogAddressScan::on_pushButtonScan_clicked()
{
    if(!_scanning)
    {
        startScan();
    }
    else
    {
        stopScan();
    }
}

///
/// \brief DialogAddressScan::startScan
///
void DialogAddressScan::startScan()
{
    _scanning = true;

    clearTableView();
    clearLogView();
    clearScanTime();
    clearProgress();

    sendReadRequest();
    _scanTimer.start(1000);
}

///
/// \brief DialogAddressScan::stopScan
///
void DialogAddressScan::stopScan()
{
    _scanning = false;
    _scanTimer.stop();
}

///
/// \brief DialogAddressScan::sendReadRequest
///
void DialogAddressScan::sendReadRequest()
{
    const auto deviceId = ui->lineEditSlaveAddress->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();
    const auto address = pointAddress - 1 + _requestCount++;
    _modbusClient.sendReadRequest(pointType, address, 1, deviceId, 0);
}

///
/// \brief DialogAddressScan::clearTableView
///
void DialogAddressScan::clearTableView()
{
    const auto length = ui->lineEditLength->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();

    ModbusDataUnit data(pointType, pointAddress, length);
    _viewModel = QSharedPointer<TableViewItemModel>(new TableViewItemModel(data, 8, this));
    ui->tableView->setModel(_viewModel.get());

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(80);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

///
/// \brief DialogAddressScan::clearLogView
///
void DialogAddressScan::clearLogView()
{
    _logItems.clear();
    _logModel = QSharedPointer<LogViewItemModel>(new LogViewItemModel(_logItems, this));
    _proxyLogModel = QSharedPointer<LogViewItemProxyModel>(new LogViewItemProxyModel(this));
    _proxyLogModel->setSourceModel(_logModel.get());
    ui->logView->setModel(_proxyLogModel.get());
}

///
/// \brief DialogAddressScan::clearScanTime
///
void DialogAddressScan::clearScanTime()
{
    _scanTime = 0;
    ui->labelElapsed->setText("00:00:00");
}

///
/// \brief DialogAddressScan::clearProgress
///
void DialogAddressScan::clearProgress()
{
    _requestCount = 0;
    ui->progressBar->setValue(0);
}

///
/// \brief DialogAddressScan::updateProgress
///
void DialogAddressScan::updateProgress()
{
    const auto length = ui->lineEditLength->value<int>();
    const int progress = 100.0 * _requestCount / length;
    ui->progressBar->setValue(progress);
}

///
/// \brief DialogAddressScan::updateTableView
/// \param pointAddress
/// \param value
///
void DialogAddressScan::updateTableView(int pointAddress, quint16 value)
{
    if(!_viewModel) return;
    for(int i = 0; i < _viewModel->rowCount(); i++)
    {
        for(int j = 0; j < _viewModel->columnCount(); j++)
        {
            const auto index = _viewModel->index(i, j);
            if(_viewModel->data(index, Qt::UserRole).toInt() == pointAddress)
            {
                _viewModel->setData(index, value, Qt::DisplayRole);
                return;
            }
        }
    }
}

void DialogAddressScan::updateLogView(const QModbusRequest& request)
{
    const auto deviceId = ui->lineEditSlaveAddress->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();

    quint16 pointAddress;
    request.decodeData(&pointAddress);
    const auto address = formatAddress(pointType, pointAddress);

    QByteArray rawData;
    rawData.push_back(deviceId);
    rawData.push_back(request.functionCode() | ( request.isException() ? QModbusPdu::ExceptionByte : 0));
    rawData.push_back(request.data());

    QStringList textData;
    for(auto&& c : rawData)
        textData.append(QString("%1").arg(QString::number((uchar)c), 3, '0'));

    LogViewItem item;
    item.IsRequest = true;
    item.IsValid = true;
    item.PointAddress = pointAddress;
    item.Text = QString("[%1] << [%2]").arg(address, textData.join(' '));

    _logItems.push_back(item);
    _logModel->update();

    ui->logView->scrollTo(_proxyLogModel->index(_logItems.size() - 1, 0), QAbstractItemView::PositionAtBottom);
}

///
/// \brief DialogAddressScan::updateLogView
/// \param reply
///
void DialogAddressScan::updateLogView(const QModbusReply* reply)
{
    if(!reply)
        return;

    const auto deviceId = ui->lineEditSlaveAddress->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = reply->property("RequestData").value<QModbusDataUnit>().startAddress() + 1;
    const auto address = formatAddress(pointType, pointAddress);
    const auto pdu = reply->rawResult();

    QByteArray rawData;
    rawData.push_back(deviceId);
    rawData.push_back(pdu.functionCode() | ( pdu.isException() ? QModbusPdu::ExceptionByte : 0));
    rawData.push_back(pdu.data());

    QStringList textData;
    for(auto&& c : rawData)
        textData.append(QString("%1").arg(QString::number((uchar)c), 3, '0'));

    LogViewItem item;
    item.IsRequest = false;
    item.IsValid = (reply->error() == QModbusDevice::NoError);
    item.PointAddress = pointAddress;
    item.Text = QString("[%1] >> [%2]").arg(address, textData.join(' '));

    _logItems.push_back(item);
    _logModel->update();

    ui->logView->scrollTo(_proxyLogModel->index(_logItems.size() - 1, 0), QAbstractItemView::PositionAtBottom);
}
