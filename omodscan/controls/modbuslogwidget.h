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
    void append(const ModbusMessage* data);
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
    QQueue<const ModbusMessage*> _items;
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
    void addItem(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request);
    const ModbusMessage* itemAt(const QModelIndex& index);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    int rowLimit() const;
    void setRowLimit(int val);

private:
    DataDisplayMode _dataDisplayMode;
};

#endif // MODBUSLOGWIDGET_H
