#include <QMenu>
#include <QEvent>
#include <QStyle>
#include <QClipboard>
#include <QTextDocument>
#include <QApplication>
#include "fontutils.h"
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

    const auto item = _items.at(index.row());
    switch(role)
    {
        case Qt::DisplayRole:
            return QString(R"(
                    <span style="color:#444444">%1</span>
                    <b style="color:%2">%3</b>
                    <span>%4</span>
                )")
                .arg(item->timestamp().toString(Qt::ISODateWithMs),
                     item->isRequest() ? "#0066cc" : "#009933",
                     item->isRequest() ? "[Tx] ←" : "[Rx] →",
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
void ModbusLogModel::append(QSharedPointer<const ModbusMessage> data)
{
    if(data == nullptr) return;

    while(rowCount() >= _rowLimit)
    {
        beginRemoveRows(QModelIndex(), 0, 0);
        _items.removeFirst();
        endRemoveRows();
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
    _items.clear();
}

///
/// \brief ModbusLogWidget::ModbusLogWidget
/// \param parent
///
ModbusLogWidget::ModbusLogWidget(QWidget* parent)
    : QListView(parent)
    , _autoscroll(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setFont(defaultMonospaceFont());
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegate(new HtmlDelegate(this));
    setModel(new ModbusLogModel(this));

    QIcon copyIcon = QIcon::fromTheme("edit-copy");
    if (copyIcon.isNull()) {
        copyIcon = style()->standardIcon(QStyle::SP_FileIcon);
    }

    _copyAct = new QAction(copyIcon, tr("Copy Text"), this);
    _copyAct->setShortcut(QKeySequence::Copy);
    _copyAct->setShortcutContext(Qt::WidgetShortcut);
    _copyAct->setShortcutVisibleInContextMenu(true);
    addAction(_copyAct);

    connect(_copyAct, &QAction::triggered, this, [this]() {
        QModelIndex index = currentIndex();
        if (index.isValid()) {
            QTextDocument doc;
            doc.setHtml(index.data(Qt::DisplayRole).toString());
            QApplication::clipboard()->setText(doc.toPlainText());
        }
    });

    _copyBytesAct = new QAction(tr("Copy Bytes"), this);
    _copyBytesAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    _copyBytesAct->setShortcutContext(Qt::WidgetShortcut);
    _copyBytesAct->setShortcutVisibleInContextMenu(true);
    addAction(_copyBytesAct);

    connect(_copyBytesAct, &QAction::triggered, this, [this]() {
        QModelIndex index = currentIndex();
        if (index.isValid()) {
            auto msg = index.data(Qt::UserRole).value<QSharedPointer<const ModbusMessage>>();
            if (msg) QApplication::clipboard()->setText(msg->toString(dataDisplayMode()));
        }
    });

    connect(this, &QWidget::customContextMenuRequested,
            this, &ModbusLogWidget::on_customContextMenuRequested);
    connect(model(), &ModbusLogModel::rowsInserted,
            this, [&]{
        if(_autoscroll) scrollToBottom();
    });
}

///
/// \brief ModbusLogWidget::changeEvent
/// \param event
///
void ModbusLogWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        update();
        _copyAct->setText(tr("Copy Text"));
        _copyBytesAct->setText(tr("Copy Bytes"));
    }
    QListView::changeEvent(event);
}

///
/// \brief ModbusLogWidget::clear
///
void ModbusLogWidget::clear()
{
    if(model())
        ((ModbusLogModel*)model())->clear();
}

///
/// \brief ModbusLogWidget::rowCount
/// \return
///
int ModbusLogWidget::rowCount() const
{
    return model() ? model()->rowCount() : 0;
}

///
/// \brief ModbusLogWidget::index
/// \param row
/// \return
///
QModelIndex ModbusLogWidget::index(int row)
{
    return model() ? model()->index(row, 0) : QModelIndex();
}

///
/// \brief ModbusLogWidget::addItem
/// \param pdu
/// \param protocol
/// \param deviceId
/// \param transactionId
/// \param timestamp
/// \param request
/// \return
///
QSharedPointer<const ModbusMessage> ModbusLogWidget::addItem(const QModbusPdu& pdu, ModbusMessage::ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp, bool request)
{
    QSharedPointer<const ModbusMessage> msg;
    if(model())
    {
        msg = ModbusMessage::create(pdu, protocol, deviceId, timestamp, request);
        if(protocol == ModbusMessage::Tcp)
            ((QModbusAduTcp*)msg->adu())->setTransactionId(transactionId);

        ((ModbusLogModel*)model())->append(msg);
    }
    return msg;
}

///
/// \brief ModbusLogWidget::itemAt
/// \param index
/// \return
///
QSharedPointer<const ModbusMessage> ModbusLogWidget::itemAt(const QModelIndex& index)
{
    if(!index.isValid())
        return nullptr;

    return model() ?
           model()->data(index, Qt::UserRole).value<QSharedPointer<const ModbusMessage>>() :
           nullptr;
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

    if(model()) {
        ((ModbusLogModel*)model())->update();
    }
}

///
/// \brief ModbusLogWidget::rowLimit
/// \return
///
int ModbusLogWidget::rowLimit() const
{
    return model() ? ((ModbusLogModel*)model())->rowLimit() : 0;
}

///
/// \brief ModbusLogWidget::setRowLimit
/// \param val
///
void ModbusLogWidget::setRowLimit(int val)
{
    if(model()) {
        ((ModbusLogModel*)model())->setRowLimit(val);
    }
}

///
/// \brief ModbusLogWidget::autoscroll
/// \return
///
bool ModbusLogWidget::autoscroll() const
{
    return _autoscroll;
}

///
/// \brief ModbusLogWidget::setAutoscroll
/// \param on
///
void ModbusLogWidget::setAutoscroll(bool on)
{
    _autoscroll = on;
}

///
/// \brief ModbusLogWidget::backgroundColor
/// \return
///
QColor ModbusLogWidget::backgroundColor() const
{
    return palette().color(QPalette::Base);
}

///
/// \brief ModbusLogWidget::setBackGroundColor
/// \param clr
///
void ModbusLogWidget::setBackGroundColor(const QColor& clr)
{
    auto pal = palette();
    pal.setColor(QPalette::Base, clr);
    pal.setColor(QPalette::Window, clr);
    setPalette(pal);
}

///
/// \brief ModbusLogWidget::on_customContextMenuRequested
/// \param pos
///
void ModbusLogWidget::on_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    menu.addAction(_copyAct);
    menu.addAction(_copyBytesAct);
    menu.exec(viewport()->mapToGlobal(pos));
}
