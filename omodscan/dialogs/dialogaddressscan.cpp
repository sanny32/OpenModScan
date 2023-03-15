#include <QtCore>
#include <QtWidgets>
#include <QtPrintSupport>
#include "byteorderutils.h"
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
/// \brief formatValue
/// \param pointType
/// \param value
/// \param mode
/// \param order
/// \return
///
QString formatValue(QModbusDataUnit::RegisterType pointType, quint16 value, DataDisplayMode mode, ByteOrder order)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("%1").arg(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            switch(mode)
            {
                case DataDisplayMode::Hex:
                    result = QStringLiteral("%1H").arg(value, 4, 16, QLatin1Char('0'));
                break;

                default:
                    result = QString("%1").arg(value);
                break;
            }
        }
        break;
        default:
        break;
    }
    return result.toUpper();
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
        {
            const auto mode = _hexView ? DataDisplayMode::Hex : DataDisplayMode::Decimal;
            return _data.hasValue(idx) ? formatValue(_data.registerType(), _data.value(idx), mode, _byteOrder) : "-";
        }

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
    if(value.userType() == qMetaTypeId<QVector<quint16>>())
    {
        const auto values = value.value<QVector<quint16>>();
        for(int i = 0; i < values.size(); i++)
            _data.setValue(idx + i, values.at(i));

        emit dataChanged(index, this->index(rowCount() - 1, columnCount() - 1), QVector<int>() << Qt::DisplayRole);
    }
    else
    {
        _data.setValue(idx, value.toUInt());
        emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
    }

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
                    const auto pointType = _data.registerType();
                    const auto pointAddress = _data.startAddress();
                    const auto addressFrom = pointAddress + section * _columns;
                    const auto addressTo = pointAddress + qMin<quint16>(length - 1, (section + 1) * _columns - 1);
                    return QString("%1-%2").arg(formatAddress(pointType, addressFrom), formatAddress(pointType, addressTo));
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
            return item.IsRequest ? QVariant() : QColor(0xDCDCDC);

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

    ui->comboBoxPointType->setCurrentPointType(dd.PointType);
    ui->lineEditStartAddress->setPaddingZeroes(true);
    ui->lineEditStartAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditLength->setInputRange(1, 65530);
    ui->lineEditStartAddress->setValue(dd.PointAddress);
    ui->lineEditSlaveAddress->setValue(dd.DeviceId);
    ui->lineEditLength->setValue(999);
    ui->tabWidget->setCurrentIndex(0);
    ui->checkBoxHexView->setChecked(mode == DataDisplayMode::Hex);
    ui->comboBoxByteOrder->setCurrentByteOrder(order);

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
    if(!_viewModel)
        return;

    _viewModel->setHexView(on);
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
}

///
/// \brief DialogAddressScan::on_comboBoxByteOrder_byteOrderChanged
/// \param order
///
void DialogAddressScan::on_comboBoxByteOrder_byteOrderChanged(ByteOrder order)
{
    if(!_viewModel)
        return;

    _viewModel->setByteOrder(order);
}

///
/// \brief DialogAddressScan::on_modbusRequest
/// \param requestId
/// \param request
///
void DialogAddressScan::on_modbusRequest(int requestId, const QModbusRequest& request)
{
    if(requestId != -1)
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

    if(-1 != reply->property("RequestId").toInt())
    {
        return;
    }

    updateProgress();
    updateLogView(reply);

    if (reply->error() == QModbusDevice::NoError)
        updateTableView(reply->result().startAddress() + 1, reply->result().values());

    if(ui->lineEditLength->value<int>() <= _requestCount)
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
/// \brief DialogAddressScan::startScan
///
void DialogAddressScan::startScan()
{
    if(_scanning)
        return;

    if(_modbusClient.state() != QModbusDevice::ConnectedState)
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
    const auto address = pointAddress - 1 + _requestCount;

    if(address >= ModbusLimits::addressRange().to())
        stopScan();
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

    ModbusDataUnit data(pointType, pointAddress, length);
    _viewModel = QSharedPointer<TableViewItemModel>(new TableViewItemModel(data, 10, this));
    _viewModel->setHexView(ui->checkBoxHexView->isChecked());
    _viewModel->setByteOrder(ui->comboBoxByteOrder->currentByteOrder());
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
    _proxyLogModel->setShowValid(ui->checkBoxShowValid->isChecked());
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
/// \param values
///
void DialogAddressScan::updateTableView(int pointAddress, QVector<quint16> values)
{
    if(!_viewModel) return;
    for(int i = 0; i < _viewModel->rowCount(); i++)
    {
        for(int j = 0; j < _viewModel->columnCount(); j++)
        {
            const auto index = _viewModel->index(i, j);
            if(_viewModel->data(index, Qt::UserRole).toInt() == pointAddress)
            {
                _viewModel->setData(index, QVariant::fromValue(values), Qt::DisplayRole);
                return;
            }
        }
    }
}

///
/// \brief DialogAddressScan::updateLogView
/// \param request
///
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

///
/// \brief DialogAddressScan::exportPdf
/// \param filename
///
void DialogAddressScan::exportPdf(const QString& filename)
{
    PdfExporter exporter(_viewModel.get(),
                         ui->lineEditStartAddress->text(),
                         ui->lineEditLength->text(),
                         ui->lineEditSlaveAddress->text(),
                         ui->comboBoxPointType->currentText(),
                         ui->spinBoxRegsOnQuery->text(),
                         this);

    exporter.exportPdf(filename);
}

///
/// \brief DialogAddressScan::exportCsv
/// \param filename
///
void DialogAddressScan::exportCsv(const QString& filename)
{
    CsvExporter exporter(_viewModel.get(),
                         ui->lineEditStartAddress->text(),
                         ui->lineEditLength->text(),
                         ui->lineEditSlaveAddress->text(),
                         ui->comboBoxPointType->currentText(),
                         ui->spinBoxRegsOnQuery->text(),
                         this);

    exporter.exportCsv(filename);
}

///
/// \brief PdfExporter::PdfExporter
/// \param model
/// \param startAddress
/// \param length
/// \param devId
/// \param pointType
/// \param parent
///
PdfExporter::PdfExporter(QAbstractTableModel* model,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         QObject* parent)
    : QObject(parent)
    ,_model(model)
    ,_startAddress(startAddress)
    ,_length(length)
    ,_deviceId(devId)
    ,_pointType(pointType)
    ,_regsOnQuery(regsOnQuery)
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

    const auto text1 = QString(tr("Device Id: %1\tLength: %2\nPoint Type: [%3]")).arg(_deviceId, _length, _pointType);
    auto rc1 = painter.boundingRect(_pageRect, Qt::TextWordWrap, text1);

    const auto text2 = QString(tr("Start Address: %1\nRegisters on Query: %2")).arg(_startAddress, _regsOnQuery);
    auto rc2 = painter.boundingRect(_pageRect, Qt::TextWordWrap, text2);

    rcTime.moveTopRight({ _pageRect.right(), 10 });
    rc1.moveLeft(rc2.right() + 40);

    painter.drawText(rcTime, Qt::TextSingleLine, textTime);
    painter.drawText(rc2, Qt::TextWordWrap, text2);
    painter.drawText(rc1, Qt::TextWordWrap, text1);

    yPos += qMax(rc1.height(), rc2.height()) + 20;
}

///
/// \brief PdfExporter::paintPageFooter
/// \param painter
///
void PdfExporter::paintPageFooter(QPainter& painter)
{
    const auto textNumber = QString::number(_pageNumber);
    auto rc = painter.boundingRect(_pageRect, Qt::TextSingleLine, textNumber);

    rc.moveTopRight({ _pageRect.right(), _pageRect.bottom() + 10 });
    painter.drawText(rc, Qt::TextSingleLine, textNumber);
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
CsvExporter::CsvExporter(QAbstractTableModel* model,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         QObject* parent)
    : QObject(parent)
    ,_model(model)
    ,_startAddress(startAddress)
    ,_length(length)
    ,_deviceId(devId)
    ,_pointType(pointType)
    ,_regsOnQuery(regsOnQuery)
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
    const char* delim = ";";

    const auto header = QString("%2%1%3%1%4%1%5%1%6").arg(delim, tr("Device Id"), tr("Start Address"), tr("Length"), tr("Point Type"), tr("Registers on Query"));
    ts << header << "\n";

    const auto headerData = QString("%2%1%3%1%4%1%5%1%6").arg(delim, _deviceId, _startAddress, _length, _pointType, _regsOnQuery);
    ts << headerData << "\n";

    ts << "\n" << "Address" << delim << "Value\n";
    for(int i = 0; i < _model->rowCount(); i++)
    {
        for(int j = 0; j < _model->columnCount(); j++)
        {
            const auto address = _model->data(_model->index(i, j), Qt::ToolTipRole).toString();
            const auto value = _model->data(_model->index(i, j), Qt::DisplayRole).toString();
            ts << address << delim << value << "\n";
        }
    }
}
