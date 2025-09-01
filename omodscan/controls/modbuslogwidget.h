#ifndef MODBUSLOGWIDGET_H
#define MODBUSLOGWIDGET_H

#include <QQueue>
#include <QListView>
#include "modbusmessage.h"

class ModbusLogWidget;

///
/// \brief The ModbusLogModel class
///
class  ModbusLogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ModbusLogModel(ModbusLogWidget* parent);
    ~ModbusLogModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    void clear();
    void append(QSharedPointer<const ModbusMessage> data);
    void update(){
        emit dataChanged(index(0), index(_items.size() - 1));
    }

    int rowLimit() const;
    void setRowLimit(int val);

private:
    void deleteItems();

private:
    int _rowLimit = 30;
    ModbusLogWidget* _parentWidget;
    QQueue<QSharedPointer<const ModbusMessage>> _items;
};

///
/// \brief The ModbusLogWidget class
///
class ModbusLogWidget : public QListView
{
    Q_OBJECT
public:
    explicit ModbusLogWidget(QWidget* parent = nullptr);

    void clear();

    int rowCount() const;
    QModelIndex index(int row);

    QSharedPointer<const ModbusMessage> addItem(const QModbusPdu& pdu, ModbusMessage::ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp, bool request);
    QSharedPointer<const ModbusMessage> itemAt(const QModelIndex& index);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    int rowLimit() const;
    void setRowLimit(int val);

    bool autoscroll() const;
    void setAutoscroll(bool on);

    QColor backgroundColor() const;
    void setBackGroundColor(const QColor& clr);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_customContextMenuRequested(const QPoint &pos);

private:
    bool _autoscroll;
    QAction* _copyAct;
    QAction* _copyBytesAct;
    DataDisplayMode _dataDisplayMode;
};

#endif // MODBUSLOGWIDGET_H
