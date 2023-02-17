#ifndef DIALOGADDRESSSCAN_H
#define DIALOGADDRESSSCAN_H

#include <QDialog>
#include <QAbstractTableModel>
#include "modbusdataunit.h"
#include "modbusclient.h"
#include "displaydefinition.h"

namespace Ui {
class DialogAddressScan;
}

///
/// \brief The TableViewItemModel class
///
class TableViewItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableViewItemModel(const ModbusDataUnit& data, int columns, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    int _columns;
    ModbusDataUnit _data;
};


///
/// \brief The DialogAddressScan class
///
class DialogAddressScan : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddressScan(const DisplayDefinition& dd, ModbusClient& client, QWidget *parent = nullptr);
    ~DialogAddressScan();

private slots:
    void on_awake();
    void on_timeout();
    void on_modbusReply(QModbusReply* reply);
    void on_lineEditStartAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditSlaveAddress_valueChanged(const QVariant&);
    void on_comboBoxPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_pushButtonScan_clicked();

private:
    void startScan();
    void stopScan();

    void sendReadRequest();

    void clearTableView();
    void clearLogView();

    void updateTableView(int pointAddress, quint16 value);
    void updateLogView(int pointAddress, const QString& status);

private:
    Ui::DialogAddressScan *ui;

private:
    int _requestCount = 0;
    bool _scanning = false;
    ModbusClient& _modbusClient;
    QSharedPointer<TableViewItemModel> _viewModel;
};

#endif // DIALOGADDRESSSCAN_H
