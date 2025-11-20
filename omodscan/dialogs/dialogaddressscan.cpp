#include <QtCore>
#include <QtWidgets>
#include <QtPrintSupport>
#include "fontutils.h"
#include "modbuslimits.h"
#include "dialogaddressscan.h"
#include "ui_dialogaddressscan.h"

///
/// \brief TableViewItemModel::TableViewItemModel
/// \param parent
///
TableViewItemModel::TableViewItemModel(QObject* parent)
    : QAbstractTableModel(parent)
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
            return formatAddress(_data.registerType(), getAddress(idx), _addressSpace, false);

        case Qt::DisplayRole:
        {
            QVariant outValue;
            const auto value = _data.value(idx);
            const auto pointType = _data.registerType();
            auto result = _hexView ? formatHexValue(pointType, value, _byteOrder, outValue) :
                                    formatUInt16Value(pointType, value, _byteOrder, _showLeadingZeros, outValue);

            return _data.hasValue(idx) ? result.remove('<').remove('>') : "-";
        }

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::BackgroundRole:
            return _data.hasValue(idx) ?
                       QVariant() :
                       parentWidget->palette().color(QPalette::Disabled, QPalette::Base);

        case Qt::UserRole:
            return getAddress(idx);

        case Qt::UserRole + 1:
            return  _data.hasValue(idx) ? _data.value(idx) : QVariant();
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
    if (!index.isValid())
    {
        return false;
    }

    switch(role)
    {
        case Qt::DisplayRole:
        {
            const auto idx = index.row() * _columns + index.column();
            if(value.userType() == qMetaTypeId<QVector<quint16>>())
            {
                const auto values = value.value<QVector<quint16>>();
                for(int i = 0; i < values.size(); i++)
                    _data.setValue(idx + i, values.at(i));

                emit dataChanged(index, this->index(rowCount() - 1, columnCount() - 1), QVector<int>() << role);
            }
            else
            {
                _data.setValue(idx, value.toUInt());
                emit dataChanged(index, index, QVector<int>() << role);
            }
        }
        return true;
    }

    return false;
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
                    const auto pointType = _data.registerType();
                    const auto pointAddress = getAddress(0);
                    const auto addressFrom = pointAddress + section * _columns;
                    const auto addressTo = pointAddress + qMin<quint16>(length - 1, (section + 1) * _columns - 1);
                    return QString("%1-%2").arg(formatAddress(pointType, addressFrom, _addressSpace, _hexAddress), formatAddress(pointType, addressTo, _addressSpace, _hexAddress));
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
/// \brief TableViewItemModel::addressBase
/// \return
///
AddressBase TableViewItemModel::addressBase() const
{
    return _addressBase;
}

///
/// \brief TableViewItemModel::setAddressBase
/// \param base
///
void TableViewItemModel::setAddressBase(AddressBase base)
{
    _addressBase = base;
}

///
/// \brief TableViewItemModel::addressSpace
/// \return
///
AddressSpace TableViewItemModel::addressSpace() const
{
    return _addressSpace;
}

///
/// \brief TableViewItemModel::setAddressSpace
/// \param space
///
void TableViewItemModel::setAddressSpace(AddressSpace space)
{
    _addressSpace = space;
}

///
/// \brief TableViewItemModel::showLeadingZeros
/// \return
///
bool TableViewItemModel::showLeadingZeros() const
{
    return _showLeadingZeros;
}

///
/// \brief TableViewItemModel::setShowLeadingZeros
/// \param value
///
void TableViewItemModel::setShowLeadingZeros(bool value)
{
    _showLeadingZeros = value;
}

///
/// \brief TableViewItemModel::getAddress
/// \param idx
/// \return
///
int TableViewItemModel::getAddress(int idx) const
{
    return _data.startAddress() + idx + (_addressBase == AddressBase::Base0 ? 0 : 1);
}

///
/// \brief LogViewModel::LogViewModel
/// \param parent
///
LogViewModel::LogViewModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

///
/// \brief LogViewModel::~LogViewModel
///
LogViewModel::~LogViewModel()
{
    void deleteItems();
}

///
/// \brief LogViewModel::rowCount
/// \param parent
/// \return
///
int LogViewModel::rowCount(const QModelIndex&) const
{
    return _items.size();
}

///
/// \brief LogViewModel::data
/// \param index
/// \param role
/// \return
///
QVariant LogViewModel::data(const QModelIndex& index, int role) const
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
        {
            const DataDisplayMode mode = _hexView ? DataDisplayMode::Hex : DataDisplayMode::UInt16;
            const auto addr = item.Addr + (_addressBase == AddressBase::Base1 ? 1 : 0);
            return QString("[%1] %2 [%3]").arg(formatAddress(item.Type, addr, _addressSpace, _hexAddress),
                                               item.Msg->isRequest() ? "<<" : ">>",
                                               item.Msg->toString(mode));
        }

        case Qt::BackgroundRole:
            return item.Msg->isRequest() ? QVariant() : QColor(0xDCDCDC);

        case Qt::UserRole:
            return QVariant::fromValue(item.Msg);
    }

    return QVariant();
}

///
/// \brief LogViewModel::addressBase
/// \return
///
AddressBase LogViewModel::addressBase() const
{
    return _addressBase;
}

///
/// \brief LogViewModel::setAddressBase
/// \param base
///
void LogViewModel::setAddressBase(AddressBase base)
{
    _addressBase = base;
}

///
/// \brief LogViewModel::addressSpace
/// \return
///
AddressSpace LogViewModel::addressSpace() const
{
    return _addressSpace;
}

///
/// \brief LogViewModel::setAddressSpace
/// \param space
///
void LogViewModel::setAddressSpace(AddressSpace space)
{
    _addressSpace = space;
}

///
/// \brief LogViewModel::deleteItems
///
void LogViewModel::deleteItems()
{
    _items.clear();
}

///
/// \brief LogViewProxyModel::LogViewProxyModel
/// \param parent
///
LogViewProxyModel::LogViewProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    ,_showValid(false)
{
}

///
/// \brief LogViewProxyModel::filterAcceptsRow
/// \param source_row
/// \param source_parent
/// \return
///
bool LogViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const auto index = sourceModel()->index(source_row, 0, source_parent);
    const auto msg = sourceModel()->data(index, Qt::UserRole).value<QSharedPointer<const ModbusMessage>>();
    return _showValid ? msg->isValid() && !msg->isRequest() && !msg->isException() : true;
}

///
/// \brief DialogAddressScan::DialogAddressScan
/// \param dd
/// \param mode
/// \param order
/// \param client
/// \param parent
///
DialogAddressScan::DialogAddressScan(const DisplayDefinition& dd, DataDisplayMode mode, ByteOrder order, ModbusClient& client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddressScan)
    ,_modbusClient(client)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowMaximizeButtonHint);

    auto viewModel = new TableViewItemModel(this);
    viewModel->setHexAddress(dd.HexAddress);
    viewModel->setAddressBase(dd.ZeroBasedAddress ? AddressBase::Base0 : AddressBase::Base1);
    viewModel->setAddressSpace(dd.AddrSpace);
    viewModel->setShowLeadingZeros(dd.LeadingZeros);

    auto logModel = new LogViewModel(this);
    logModel->setHexAddress(dd.HexAddress);
    logModel->setAddressBase(dd.ZeroBasedAddress ? AddressBase::Base0 : AddressBase::Base1);
    logModel->setAddressSpace(dd.AddrSpace);

    auto proxyLogModel = new LogViewProxyModel(this);
    proxyLogModel->setSourceModel(logModel);

    ui->tableView->setModel(viewModel);
    ui->logView->setModel(proxyLogModel);

    ui->comboBoxPointType->setCurrentPointType(dd.PointType);
    ui->comboBoxAddressBase->setCurrentAddressBase(dd.ZeroBasedAddress ? AddressBase::Base0 : AddressBase::Base1);
    ui->lineEditStartAddress->setLeadingZeroes(true);
    ui->lineEditStartAddress->setInputMode(dd.HexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditStartAddress->setInputRange(ModbusLimits::addressRange(dd.ZeroBasedAddress));
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditLength->setInputRange(2, 65530);
    ui->lineEditStartAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(999);
    ui->tabWidget->setCurrentIndex(0);
    ui->checkBoxHexView->setChecked(mode == DataDisplayMode::Hex);
    ui->comboBoxByteOrder->setCurrentByteOrder(order);
    ui->info->setShowTimestamp(false);
    ui->logView->setFont(defaultMonospaceFont());

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogAddressScan::on_awake);

    connect(&_scanTimer, &QTimer::timeout, this, &DialogAddressScan::on_timeout);
    connect(&_modbusClient, &ModbusClient::modbusReply, this, &DialogAddressScan::on_modbusReply);
    connect(&_modbusClient, &ModbusClient::modbusRequest, this, &DialogAddressScan::on_modbusRequest);
    connect(proxyLogModel->sourceModel(), &LogViewModel::rowsInserted, ui->logView, &QListView::scrollToBottom);

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
/// \brief DialogAddressScan::changeEvent
/// \param event
///
void DialogAddressScan::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);

    QDialog::changeEvent(event);
}

///
/// \brief DialogAddressScan::on_awake
///
void DialogAddressScan::on_awake()
{
    ui->lineEditStartAddress->setEnabled(!_scanning);
    ui->lineEditLength->setEnabled(!_scanning);
    ui->lineEditSlaveAddress->setEnabled(!_scanning);
    ui->spinBoxRegsOnQuery->setEnabled(!_scanning);
    ui->comboBoxPointType->setEnabled(!_scanning);
    ui->pushButtonExport->setEnabled(_finished);
    ui->progressBar->setVisible(_scanning);
    ui->pushButtonScan->setText(_scanning ? tr("Stop") : tr("Scan"));
    ui->pushButtonFind->setEnabled(ui->tableView->model()->rowCount() > 0);
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
/// \brief DialogAddressScan::on_checkBoxHexView_toggled
/// \param on
///
void DialogAddressScan::on_checkBoxHexView_toggled(bool on)
{  
    ((TableViewItemModel*)ui->tableView->model())->setHexView(on);
    ((LogViewProxyModel*)ui->logView->model())->setHexView(on);
    ui->info->setDataDisplayMode(on ? DataDisplayMode::Hex : DataDisplayMode::UInt16);
    ui->lineEditToFind->setInputMode(on ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
}

///
/// \brief DialogAddressScan::on_checkBoxShowValid_toggled
/// \param on
///
void DialogAddressScan::on_checkBoxShowValid_toggled(bool on)
{  
    ((LogViewProxyModel*)ui->logView->model())->setShowValid(on);
}

///
/// \brief DialogAddressScan::on_lineEditStartAddress_valueChanged
/// \param value
///
void DialogAddressScan::on_lineEditStartAddress_valueChanged(const QVariant& value)
{
    Q_UNUSED(value)

    clearTableView();
    clearLogView();
}

///
/// \brief DialogAddressScan::on_lineEditLength_valueChanged
/// \param value
///
void DialogAddressScan::on_lineEditLength_valueChanged(const QVariant& value)
{
    Q_UNUSED(value)

    clearTableView();
    clearLogView();
}

///
/// \brief DialogAddressScan::on_lineEditToFind_valueChanged
/// \param value
///
void DialogAddressScan::on_lineEditToFind_valueChanged(const QVariant& value)
{
    Q_UNUSED(value)

    ui->tableView->selectionModel()->clearSelection();
}

///
/// \brief DialogAddressScan::on_comboBoxPointType_pointTypeChanged
/// \param pointType
///
void DialogAddressScan::on_comboBoxPointType_pointTypeChanged(QModbusDataUnit::RegisterType pointType)
{
    Q_UNUSED(pointType)

    clearTableView();
    clearLogView();
}

///
/// \brief DialogAddressScan::on_comboBoxAddressBase_addressBaseChanged
/// \param base
///
void DialogAddressScan::on_comboBoxAddressBase_addressBaseChanged(AddressBase base)
{
    const auto addr = ui->lineEditStartAddress->value<int>();

    ui->lineEditStartAddress->setInputRange(ModbusLimits::addressRange(base == AddressBase::Base0));
    ui->lineEditStartAddress->setValue(base == AddressBase::Base1 ? qMax(1, addr + 1) : qMax(0, addr - 1));

    ((TableViewItemModel*)ui->tableView->model())->setAddressBase(base);
    ((LogViewProxyModel*)ui->logView->model())->setAddressBase(base);

    clearTableView();
    clearLogView();
}

///
/// \brief DialogAddressScan::on_comboBoxByteOrder_byteOrderChanged
/// \param order
///
void DialogAddressScan::on_comboBoxByteOrder_byteOrderChanged(ByteOrder order)
{
    ui->info->setByteOrder(order);
    ((TableViewItemModel*)ui->tableView->model())->setByteOrder(order);
}

///
/// \brief DialogAddressScan::on_logView_clicked
/// \param index
///
void DialogAddressScan::on_logView_clicked(const QModelIndex &index)
{
    if(!index.isValid())
    {
        ui->info->clear();
        return;
    }

    auto proxyLogModel = ((LogViewProxyModel*)ui->logView->model());
    auto msg = proxyLogModel->data(index, Qt::UserRole).value<QSharedPointer<const ModbusMessage>>();
    ui->info->setModbusMessage(msg);
}

///
/// \brief DialogAddressScan::on_modbusRequest
/// \param requestGroupId
/// \param msg
///
void DialogAddressScan::on_modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(requestGroupId == -1)
        updateLogView(msg);
}

///
/// \brief DialogAddressScan::on_modbusResponse
/// \param requestGroupId
/// \param msg
///
void DialogAddressScan::on_modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg)
{
    if(requestGroupId == -1)
        updateLogView(msg);
}

///
/// \brief DialogAddressScan::on_modbusReply
/// \param reply
///
void DialogAddressScan::on_modbusReply(const ModbusReply* const reply)
{
    if(!_scanning || !reply) return;

    if(-1 != reply->requestGroupId())
    {
        return;
    }

    updateProgress();

    if (reply->error() == ModbusDevice::NoError)
        updateTableView(reply->result().startAddress(), reply->result().values());

    if(_requestCount > ui->lineEditLength->value<int>()
                       + ui->spinBoxRegsOnQuery->value())
    {
        stopScan();
    }
    else
    {
        sendReadRequest();
    }
}

///
/// \brief DialogAddressScan::on_pushButtonScan_clicked
///
void DialogAddressScan::on_pushButtonScan_clicked()
{
    if(!_scanning)
        startScan();
    else
        stopScan();
}

///
/// \brief DialogAddressScan::on_pushButtonExport_clicked
///
void DialogAddressScan::on_pushButtonExport_clicked()
{
    auto filename = QFileDialog::getSaveFileName(this, QString(), windowTitle(), tr("Pdf files (*.pdf);;CSV files (*.csv)"));
    if(filename.isEmpty()) return;

    if(!filename.endsWith(".pdf", Qt::CaseInsensitive) &&
       !filename.endsWith(".csv", Qt::CaseInsensitive))
    {
        filename += ".pdf";
    }

    if(filename.endsWith(".pdf", Qt::CaseInsensitive))
        exportPdf(filename);
    else if(filename.endsWith(".csv", Qt::CaseInsensitive))
        exportCsv(filename);
}

///
/// \brief DialogAddressScan::on_pushButtonFind_clicked
///
void DialogAddressScan::on_pushButtonFind_clicked()
{
    const auto valueToFind = ui->lineEditToFind->value<int>();
    auto model = ((TableViewItemModel*)ui->tableView->model());
    auto selectionModel = ui->tableView->selectionModel();

    if(!selectionModel->hasSelection())
    {
        for(int i = 0; i < model->rowCount(); i++)
        {
            for(int j = 0; j < model->columnCount(); j++)
            {
                const auto index = model->index(i,j);

                bool ok;
                const auto value = model->data(index, Qt::UserRole + 1).toInt(&ok);
                if(!ok) continue;

                if(value == valueToFind)
                {
                    selectionModel->select(index, QItemSelectionModel::Select);
                }
            }
        }
    }

    bool found = false;
    const auto currentIndex = ui->tableView->currentIndex();
    const auto selectedIndexes = selectionModel->selectedIndexes();
    const auto  i = currentIndex.row() * model->columnCount() + currentIndex.column();
    for(auto&& index : selectedIndexes)
    {
        const int j = index.row() * model->columnCount() + index.column();
        if(j > i) {
            found = true;
            ui->tableView->scrollTo(index);
            ui->tableView->setCurrentIndex(index);
            break;
        }
    }
    if(!found && !selectedIndexes.isEmpty()) {
        ui->tableView->scrollTo(model->index(0,0));
        ui->tableView->scrollTo(selectedIndexes.first());
        ui->tableView->setCurrentIndex(selectedIndexes.first());
    }

    ui->tableView->setFocus();
}

///
/// \brief DialogAddressScan::startScan
///
void DialogAddressScan::startScan()
{
    if(_scanning)
        return;

    if(_modbusClient.state() != ModbusDevice::ConnectedState)
    {
        QMessageBox::warning(this, windowTitle(), tr("No connection to MODBUS device!"));
        return;
    }

    _scanning = true;
    _finished = false;

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
    if(!_scanning)
        return;

    _scanning = false;
    _finished = true;
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
    const auto count = ui->spinBoxRegsOnQuery->value();
    const auto addressBase = ui->comboBoxAddressBase->currentAddressBase();
    const auto address = (addressBase == AddressBase::Base0 ? pointAddress : pointAddress - 1) + _requestCount;

    if(address > ModbusLimits::addressRange().to())
    {
        stopScan();
    }
    else
    {
        _requestCount += count;
        _modbusClient.sendReadRequest(pointType, address, count, deviceId, -1);
    }
}

///
/// \brief DialogAddressScan::clearTableView
///
void DialogAddressScan::clearTableView()
{
    const auto length = ui->lineEditLength->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();
    const auto addressBase = ui->comboBoxAddressBase->currentAddressBase();

    ModbusDataUnit data(pointType, addressBase == AddressBase::Base0 ? pointAddress : pointAddress - 1, length);
    ((TableViewItemModel*)ui->tableView->model())->reset(data);

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
    ui->info->clear();
    ((LogViewProxyModel*)ui->logView->model())->clear();
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
/// \param values
///
void DialogAddressScan::updateTableView(int pointAddress, QVector<quint16> values)
{
    auto model = ui->tableView->model();

    const auto addressBase = ui->comboBoxAddressBase->currentAddressBase();
    pointAddress += (addressBase == AddressBase::Base0 ? 0 : 1);

    for(int i = 0; i < model->rowCount(); i++)
    {
        for(int j = 0; j < model->columnCount(); j++)
        {
            const auto index = model->index(i, j);
            if(model->data(index, Qt::UserRole).toInt() == pointAddress)
            {
                model->setData(index, QVariant::fromValue(values), Qt::DisplayRole);
                return;
            }
        }
    }
}

///
/// \brief DialogAddressScan::updateLogView
/// \param msg
///
void DialogAddressScan::updateLogView(QSharedPointer<const ModbusMessage> msg)
{
    if(!msg)
        return;

    quint16 pointAddress;
    msg->adu()->pdu().decodeData(&pointAddress);

    const auto addressBase = ui->comboBoxAddressBase->currentAddressBase();
    pointAddress += (addressBase == AddressBase::Base0 ? 0 : 1);

    auto proxyLogModel = ((LogViewProxyModel*)ui->logView->model());
    proxyLogModel->append(pointAddress, ui->comboBoxPointType->currentPointType(), msg);
}


///
/// \brief DialogAddressScan::exportPdf
/// \param filename
///
void DialogAddressScan::exportPdf(const QString& filename)
{
    PdfExporter exporter(ui->tableView->model(),
                         ui->comboBoxAddressBase->currentText(),
                         ui->lineEditStartAddress->text(),
                         ui->lineEditLength->text(),
                         ui->lineEditSlaveAddress->text(),
                         ui->comboBoxPointType->currentText(),
                         ui->spinBoxRegsOnQuery->text(),
                         ui->comboBoxByteOrder->currentText(),
                         this);

    exporter.exportPdf(filename);
}

///
/// \brief DialogAddressScan::exportCsv
/// \param filename
///
void DialogAddressScan::exportCsv(const QString& filename)
{
    CsvExporter exporter(ui->tableView->model(),
                         ui->comboBoxAddressBase->currentText(),
                         ui->lineEditStartAddress->text(),
                         ui->lineEditLength->text(),
                         ui->lineEditSlaveAddress->text(),
                         ui->comboBoxPointType->currentText(),
                         ui->spinBoxRegsOnQuery->text(),
                         ui->comboBoxByteOrder->currentText(),
                         this);

    exporter.exportCsv(filename);
}

///
/// \brief PdfExporter::PdfExporter
/// \param model
/// \param addressBase
/// \param startAddress
/// \param length
/// \param devId
/// \param pointType
/// \param regsOnQuery
/// \param parent
///
PdfExporter::PdfExporter(QAbstractItemModel* model,
                         const QString& addressBase,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         const QString& byteOrder,
                         QObject* parent)
    : QObject(parent)
    ,_model(model)
    ,_addressBase(addressBase)
    ,_startAddress(startAddress)
    ,_length(length)
    ,_deviceId(devId)
    ,_pointType(pointType)
    ,_regsOnQuery(regsOnQuery)
    ,_byteOrder(byteOrder)
{
    _printer = QSharedPointer<QPrinter>(new QPrinter(QPrinter::PrinterResolution));
    _printer->setOutputFormat(QPrinter::PdfFormat);
    _printer->setPageSize(QPageSize(QPageSize::A4));
    _printer->setPageOrientation(QPageLayout::Landscape);

    auto layout = _printer->pageLayout();
    const auto resolution = _printer->resolution();
    _pageRect = layout.paintRectPixels(resolution);

    const auto margin = qMax(_pageRect.width(), _pageRect.height()) * 0.05;
    layout.setMargins(QMargins(margin, margin, margin, margin));
    _pageRect.adjust(layout.margins().left(), layout.margins().top(), -layout.margins().right(), -layout.margins().bottom());
}

///
/// \brief PdfExporter::exportPdf
/// \param filename
///
void PdfExporter::exportPdf(const QString& filename)
{
     _printer->setOutputFileName(filename);

     QPainter painter;
     if(!painter.begin(_printer.get()))
     {
         auto w = qobject_cast<QWidget*>(parent());
         Q_ASSERT(w != nullptr);

         QMessageBox::warning(w, w->windowTitle(), tr("Error. Failed to write PDF file!"));
         return;
     }

     painter.setRenderHint(QPainter::Antialiasing);
     painter.setRenderHint(QPainter::TextAntialiasing);

     calcTable(painter);

     int yPos = _pageRect.top();
     paintPageHeader(yPos, painter);
     paintTable(yPos, painter);
}

///
/// \brief PdfExporter::calcTable
///
void PdfExporter::calcTable(QPainter& painter)
{
    const auto rc = _pageRect;
    const auto rows = _model->rowCount();
    const auto cols = _model->columnCount();

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(j == 0)
            {
                const auto text = _model->headerData(i, Qt::Vertical, Qt::DisplayRole).toString();
                const auto rcText = painter.boundingRect(rc, Qt::TextSingleLine, text);
                _headerWidth = qMax(_headerWidth, rcText.width() + _cx);
            }

            const auto text = _model->data(_model->index(i, j), Qt::DisplayRole).toString();
            const auto rcText = painter.boundingRect(rc, Qt::TextSingleLine, text);

            _colWidth = qMax(_colWidth, rcText.width() + _cx);
            _rowHeight = qMax(_rowHeight, rcText.height());
        }
    }
}

///
/// \brief PdfExporter::paintPageHeader
/// \param yPos
/// \param painter
///
void PdfExporter::paintPageHeader(int& yPos, QPainter& painter)
{
    const auto textTime = QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat);
    auto rcTime = painter.boundingRect(_pageRect, Qt::TextSingleLine, textTime);

    const auto text1 = QString(tr("Address Base: %1\nStart Address: %2")).arg(_addressBase, _startAddress);
    auto rc1 = painter.boundingRect(_pageRect, Qt::TextWordWrap, text1);

    const auto text2 = QString(tr("Device Id: %1\t\tLength: %2\nPoint Type: [%3]")).arg(_deviceId, _length, _pointType);
    auto rc2 = painter.boundingRect(_pageRect, Qt::TextWordWrap, text2);

    const auto text3 = QString(tr("Registers on Query: %1\nByte Order: %2")).arg(_regsOnQuery, _byteOrder);
    auto rc3 = painter.boundingRect(_pageRect, Qt::TextWordWrap, text3);

    rcTime.moveTopRight({ _pageRect.right(), 10 });
    rc2.moveLeft(rc1.right() + 40);
    rc3.moveLeft(rc2.right() + 40);

    painter.drawText(rcTime, Qt::TextSingleLine, textTime);
    painter.drawText(rc1, Qt::TextWordWrap, text1);
    painter.drawText(rc2, Qt::TextWordWrap, text2);
    painter.drawText(rc3, Qt::TextWordWrap, text3);

    yPos += qMax(rc1.height(), rc2.height()) + 20;
}

///
/// \brief PdfExporter::paintPageFooter
/// \param painter
///
void PdfExporter::paintPageFooter(QPainter& painter)
{
    const auto textNumber = QString::number(_pageNumber);
    auto rcNumber = painter.boundingRect(_pageRect, Qt::TextSingleLine, textNumber);

    rcNumber.moveTopRight({ _pageRect.right(), _pageRect.bottom() + 10 });
    painter.drawText(rcNumber, Qt::TextSingleLine, textNumber);
}

///
/// \brief PdfExporter::paintTableHeader
/// \param yPos
/// \param painter
///
void PdfExporter::paintTableHeader(int& yPos, QPainter& painter)
{
    QRect rc = _pageRect;
    rc.setTop(yPos);
    rc.setBottom(yPos + _rowHeight);

    for(int j = 0; j < _model->columnCount(); j++)
    {
        auto rcPaint = rc;
        rcPaint.setLeft(rc.left() + _headerWidth + _colWidth * j);
        rcPaint.setRight(rcPaint.left() + _colWidth);
        painter.drawText(rcPaint, Qt::AlignHCenter | Qt::TextSingleLine, _model->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString());
    }

    painter.drawLine(rc.bottomLeft(), rc.bottomRight());
    yPos += _rowHeight;
}

///
/// \brief PdfExporter::paintTableRow
/// \param yPos
/// \param painter
/// \param row
///
void PdfExporter::paintTableRow(int& yPos, QPainter& painter, int row)
{
    QRect rc = _pageRect;
    rc.setTop(yPos);

    for(int j = 0; j < _model->columnCount(); j++)
    {
        if(j == 0)
            painter.drawText(rc, Qt::TextSingleLine, _model->headerData(row, Qt::Vertical, Qt::DisplayRole).toString());

        auto rcPaint = rc;
        rcPaint.setLeft(rc.left() + _headerWidth + _colWidth * j);
        rcPaint.setRight(rcPaint.left() + _colWidth);
        painter.drawText(rcPaint, Qt::AlignHCenter | Qt::TextSingleLine, _model->data(_model->index(row, j), Qt::DisplayRole).toString());
    }

    yPos += _rowHeight;
}

///
/// \brief PdfExporter::paintTable
/// \param yPos
/// \param painter
///
void PdfExporter::paintTable(int& yPos, QPainter& painter)
{
    paintTableHeader(yPos, painter);

    QRect rc = _pageRect;
    rc.setTop(yPos);

    for(int i = 0; i < _model->rowCount(); i++)
    {
        paintTableRow(yPos, painter, i);

        if(yPos > rc.bottom() - _rowHeight)
        {
            paintVLine(rc.top() - _rowHeight, yPos + _cy, painter);
            paintPageFooter(painter);

            if(!_printer->newPage())
                break;

            _pageNumber++;
            yPos = _pageRect.top();
            paintPageHeader(yPos, painter);
            paintTableHeader(yPos, painter);
        }
    }

    if(_pageNumber > 1)
        paintPageFooter(painter);

    paintVLine(rc.top() - _rowHeight, yPos + _cy, painter);
}

///
/// \brief PdfExporter::paintVLine
/// \param top
/// \param bottom
/// \param painter
///
void PdfExporter::paintVLine(int top, int bottom, QPainter& painter)
{
    QRect rc = _pageRect;

    rc.setTop(top);
    rc.setLeft(rc.left() + _headerWidth - _cy);
    rc.setBottom(bottom);
    painter.drawLine(rc.topLeft(), rc.bottomLeft());
}

///
/// \brief CsvExporter::CsvExporter
/// \param model
/// \param startAddress
/// \param length
/// \param devId
/// \param pointType
/// \param regsOnQuery
/// \param parent
///
CsvExporter::CsvExporter(QAbstractItemModel* model,
                         const QString& addressBase,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         const QString& byteOrder,
                         QObject* parent)
    : QObject(parent)
    ,_model(model)
    ,_addressBase(addressBase)
    ,_startAddress(startAddress)
    ,_length(length)
    ,_deviceId(devId)
    ,_pointType(pointType)
    ,_regsOnQuery(regsOnQuery)
    ,_byteOrder(byteOrder)
{
}

///
/// \brief CsvExporter::exportCsv
/// \param filename
///
void CsvExporter::exportCsv(const QString& filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
        return;

    QTextStream ts(&file);
    ts.setGenerateByteOrderMark(true);

    const char* delim = ";";
    const auto header = QString("%2%1%3%1%4%1%5%1%6%1%7%1%8").arg(delim, tr("Address Base"), tr("Start Address"), tr("Device Id"), tr("Length"), tr("Point Type"), tr("Registers on Query"), tr("Byte Order"));
    ts << header << "\n";

    const auto headerData = QString("%2%1%3%1%4%1%5%1%6%1%7%1%8").arg(delim, _addressBase, _startAddress, _deviceId, _length, _pointType, _regsOnQuery, _byteOrder);
    ts << headerData << "\n";

    ts << "\n";

    for(int j = 0; j < _model->columnCount(); j++)
        ts << delim << QString("=\"%1\"").arg(_model->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString());

    for(int i = 0; i < _model->rowCount(); i++)
    {
        ts << "\n" << _model->headerData(i, Qt::Vertical, Qt::DisplayRole).toString();
        for(int j = 0; j < _model->columnCount(); j++)
        {
            ts << delim << _model->data(_model->index(i, j), Qt::DisplayRole).toString();
        }
    }
}
