#include "htmldelegate.h"
#include "modbuslogwidget.h"

///
/// \brief ModbusLogModel::ModbusLogModel
/// \param parent
///
ModbusLogModel::ModbusLogModel(ModbusLogWidget* parent)
    : QAbstractListModel(parent)
    ,_parentWidget(parent)
{
}

///
/// \brief ModbusLogModel::~ModbusLogModel
///
ModbusLogModel::~ModbusLogModel()
{
    deleteItems();
}

///
/// \brief ModbusLogModel::rowCount
/// \param parent
/// \return
///
int ModbusLogModel::rowCount(const QModelIndex&) const
{
    return _items.size();
}

///
/// \brief ModbusLogModel::data
/// \param index
/// \param role
/// \return
///
QVariant ModbusLogModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const auto& item = _items.at(index.row());
    switch(role)
    {
        case Qt::DisplayRole:
            return QString("<b>%1</b> %2 %3").arg(item->timestamp().toString(Qt::ISODateWithMs),
                                                  (item->isRequest()? "&rarr;" : "&larr;"),
                                                  item->toString(_parentWidget->dataDisplayMode()));

        case Qt::UserRole:
            return QVariant::fromValue(item);
    }

    return QVariant();
}

///
/// \brief ModbusLogModel::clear
///
void ModbusLogModel::clear()
{
    beginResetModel();
    deleteItems();
    endResetModel();
}

///
/// \brief ModbusLogModel::append
/// \param data
///
void ModbusLogModel::append(const ModbusMessage* data)
{
    if(data == nullptr) return;

    while(rowCount() >= _rowLimit)
    {
        delete _items.first();
        _items.removeFirst();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _items.push_back(data);
    endInsertRows();
}

///
/// \brief ModbusLogModel::rowLimit
/// \return
///
int ModbusLogModel::rowLimit() const
{
    return _rowLimit;
}

///
/// \brief ModbusLogModel::setRowLimit
/// \param val
///
void ModbusLogModel::setRowLimit(int val)
{
    _rowLimit = qMax(1, val);
}

///
/// \brief ModbusLogModel::deleteItems
///
void ModbusLogModel::deleteItems()
{
    for(auto&& i : _items)
        delete i;

    _items.clear();
}

///
/// \brief ModbusLogWidget::ModbusLogWidget
/// \param parent
///
ModbusLogWidget::ModbusLogWidget(QWidget* parent)
    : QListView(parent)
{
    setItemDelegate(new HtmlDelegate(this));
    setModel(new ModbusLogModel(this));

    connect(model(), &ModbusLogModel::rowsInserted,
            this, [&]{
        //scrollToBottom();
        setCurrentIndex(QModelIndex());
    });
}

///
/// \brief ModbusLogWidget::clear
///
void ModbusLogWidget::clear()
{
    ((ModbusLogModel*)model())->clear();
}

///
/// \brief ModbusLogWidget::addItem
/// \param pdu
/// \param timestamp
/// \param deviceId
/// \param request
///
void ModbusLogWidget::addItem(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request)
{
    ((ModbusLogModel*)model())->append(ModbusMessage::create(pdu, timestamp, deviceId, request));
}

///
/// \brief ModbusLogWidget::itemAt
/// \param index
/// \return
///
const ModbusMessage* ModbusLogWidget::itemAt(const QModelIndex& index)
{
    if(!index.isValid())
        return nullptr;

    return model()->data(index, Qt::UserRole).value<const ModbusMessage*>();
}

///
/// \brief ModbusLogWidget::dataDisplayMode
/// \return
///
DataDisplayMode ModbusLogWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief ModbusLogWidget::setDataDisplayMode
/// \param mode
///
void ModbusLogWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    ((ModbusLogModel*)model())->update();
}

///
/// \brief ModbusLogWidget::rowLimit
/// \return
///
int ModbusLogWidget::rowLimit() const
{
    return ((ModbusLogModel*)model())->rowLimit();
}

///
/// \brief ModbusLogWidget::setRowLimit
/// \param val
///
void ModbusLogWidget::setRowLimit(int val)
{
    ((ModbusLogModel*)model())->setRowLimit(val);
}
