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
TableViewItemModel::TableViewItemModel(const QModbusDataUnit& data, int columns, QObject* parent)
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

    switch(role)
    {
        case Qt::ToolTipRole:
            return formatAddress(_data.registerType(), _data.startAddress() + index.row() * _columns + index.column());

        case Qt::DisplayRole:
        {
            const auto idx = _data.startAddress() + index.row() * _columns + index.column();
            return QString::number(_data.value(idx));
        }

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::BackgroundRole:
        break;
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

    const auto idx = _data.startAddress() + index.row() * _columns + index.column();
    _data.setValue(idx, value.toUInt());

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
    if (section < 0 || role != Qt::DisplayRole)
    {
        return QAbstractTableModel::headerData(section, orientation, role);
    }


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

    return QAbstractTableModel::headerData(section, orientation, role);
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
/// \brief DialogAddressScan::DialogAddressScan
/// \param client
/// \param parent
///
DialogAddressScan::DialogAddressScan(ModbusClient& client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddressScan)
    ,_modbusClient(client)
{
    ui->setupUi(this);
    ui->lineEditStartAddress->setPaddingZeroes(true);
    ui->lineEditStartAddress->setInputRange(ModbusLimits::addressRange());
    ui->lineEditSlaveAddress->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditLength->setValue(10);

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &DialogAddressScan::on_awake);

    connect(&_modbusClient, &ModbusClient::modbusReply, this, &DialogAddressScan::on_modbusReply);

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
    if(_scanning)
        sendReadRequest();
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

    const auto length = ui->lineEditLength->value<int>();
    const int progress = 100.0 * _requestCount / length;
    ui->progressBar->setValue(progress);

    const auto result = reply->result();
    if (reply->error() == QModbusDevice::NoError)
    {
        const auto address = result.startAddress() + 1;
        updateTableView(address, result.value(0));
        updateLogView(address, tr("OK"));
    }
    else
    {
        const auto data = reply->property("RequestData").value<QModbusDataUnit>();
        updateLogView(data.startAddress() + 1, tr("FAILED"));
    }

    if(_requestCount == length)
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
    _requestCount = 0;

    clearTableView();
    clearLogView();
    ui->progressBar->setValue(0);

    sendReadRequest();
}

///
/// \brief DialogAddressScan::stopScan
///
void DialogAddressScan::stopScan()
{
    _scanning = false;
}

///
/// \brief DialogAddressScan::sendReadRequest
///
void DialogAddressScan::sendReadRequest()
{
    const auto deviceId = ui->lineEditSlaveAddress->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();
    _modbusClient.sendReadRequest(pointType, pointAddress - 1 + _requestCount++, 1, deviceId, 0);
}

///
/// \brief DialogAddressScan::clearTableView
///
void DialogAddressScan::clearTableView()
{
    /*const int columns = 8;
    const auto length = ui->lineEditLength->value<int>();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(columns);
    ui->tableWidget->setRowCount(qCeil(length / (double)columns));

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        const auto text = QString("+%1").arg(i);
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(text));
    }

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        const auto addressFrom = QString("%1").arg(pointAddress + i * columns, 5, 10, QLatin1Char('0'));
        const auto addressTo = QString("%1").arg(pointAddress + qMin(length - 1, (i + 1) * columns - 1), 5, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(addressFrom, addressTo)));

        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            ui->tableWidget->setCellWidget(i, j, createLineEdit(pointAddress + i * columns + j));
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    */

    const auto length = ui->lineEditLength->value<int>();
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto pointAddress = ui->lineEditStartAddress->value<int>();

    QModbusDataUnit data(pointType, pointAddress, length);
    _viewModel = QSharedPointer<TableViewItemModel>(new TableViewItemModel(data, 8, this));
    ui->tableView->setModel(_viewModel.get());

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(80);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

///
/// \brief DialogAddressScan::clearLogView
///
void DialogAddressScan::clearLogView()
{
    ui->plainTextEdit->clear();
}

///
/// \brief DialogAddressScan::updateTableView
/// \param pointAddress
/// \param value
///
void DialogAddressScan::updateTableView(int pointAddress, quint16 value)
{
    /*for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            auto lineEdit = (QLineEdit*)ui->tableWidget->cellWidget(i, j);
            if(lineEdit->property("Address").toInt() == pointAddress)
            {
                lineEdit->setEnabled(true);
                lineEdit->setReadOnly(true);
                lineEdit->setText(QString::number(value));
            }
        }
    }*/
}

///
/// \brief DialogAddressScan::updateLogView
/// \param pointAddress
/// \param status
///
void DialogAddressScan::updateLogView(int pointAddress, const QString& status)
{
    const auto pointType = ui->comboBoxPointType->currentPointType();
    const auto address = formatAddress(pointType, pointAddress);

    const auto text = QString("[%1] - %2\n").arg(address, status);
    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}
