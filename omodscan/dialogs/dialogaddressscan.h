#ifndef DIALOGADDRESSSCAN_H
#define DIALOGADDRESSSCAN_H

#include <QDialog>
#include <QTimer>
#include <QPrinter>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
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

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    int _columns;
    ModbusDataUnit _data;
};

///
/// \brief The LogViewItem class
///
struct LogViewItem
{
    quint16 PointAddress = 0;
    QString Text;
    bool IsRequest = false;
    bool IsValid = false;
};
Q_DECLARE_METATYPE(LogViewItem)

///
/// \brief The LogViewItemModel class
///
class LogViewItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LogViewItemModel(QVector<LogViewItem>& items, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    void update(){
        beginResetModel();
        endResetModel();
    }

private:
    QVector<LogViewItem>& _items;
};

///
/// \brief The LogViewItemProxyModel class
///
class LogViewItemProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit LogViewItemProxyModel(QObject* parent = nullptr);

    void setShowValid(bool on){
        _showValid = on;
    }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool _showValid;
};

///
/// \brief The PdfExporter class
///
class PdfExporter : public QObject
{
public:
    explicit PdfExporter(QAbstractTableModel* model,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType);
    void exportPdf(const QString& filename);

private:
    void calcTable(QPainter& painter);
    void paintPageHeader(int& yPos, QPainter& painter);
    void paintTableHeader(int& yPos, QPainter& painter);
    void paintTableRow(int& yPos, QPainter& painter, int row);
    void paintTable(int& yPos, QPainter& painter);

private:
    int _rowHeight = 0;
    int _colWidth = 65;
    int _headerWidth = 0;
    const int _cy = 4;
    const int _cx = 10;
    QRect _pageRect;
    QAbstractTableModel* _model;
    const QString _startAddress;
    const QString _length;
    const QString _deviceId;
    const QString _pointType;
    QSharedPointer<QPrinter> _printer;
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
    void on_modbusRequest(int requestId, const QModbusRequest& data);
    void on_lineEditStartAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditSlaveAddress_valueChanged(const QVariant&);
    void on_comboBoxPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_checkBoxShowValid_toggled(bool);
    void on_pushButtonScan_clicked();
    void on_pushButtonExport_clicked();

private:
    void startScan();
    void stopScan();

    void sendReadRequest();

    void clearTableView();
    void clearLogView();
    void clearScanTime();
    void clearProgress();

    void updateProgress();
    void updateTableView(int pointAddress, quint16 value);

    void updateLogView(const QModbusRequest& request);
    void updateLogView(const QModbusReply* reply);

    void exportPdf(const QString& filename);

private:
    Ui::DialogAddressScan *ui;

private:
    int _requestCount = 0;
    bool _scanning = false;
    bool _finished = false;
    quint64 _scanTime = 0;
    QTimer _scanTimer;
    ModbusClient& _modbusClient;
    QVector<LogViewItem> _logItems;
    QSharedPointer<LogViewItemModel> _logModel;
    QSharedPointer<LogViewItemProxyModel> _proxyLogModel;
    QSharedPointer<TableViewItemModel> _viewModel;
};

#endif // DIALOGADDRESSSCAN_H
