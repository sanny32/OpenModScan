#ifndef DIALOGADDRESSSCAN_H
#define DIALOGADDRESSSCAN_H

#include <QDialog>
#include <QTimer>
#include <QPrinter>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "modbusmessage.h"
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

    void setHexView(bool on){
        beginResetModel();
        _hexView = on;
        endResetModel();
    }

    void setByteOrder(ByteOrder order){
        beginResetModel();
        _byteOrder = order;
        endResetModel();
    }

private:
    int _columns;
    ModbusDataUnit _data;
    bool _hexView = false;
    ByteOrder _byteOrder = ByteOrder::LittleEndian;
};

///
/// \brief The LogViewModel class
///
class LogViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LogViewModel(QObject* parent = nullptr);
    ~LogViewModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    void append(quint16 addr, QModbusDataUnit::RegisterType type, const ModbusMessage* msg) {
        if(msg != nullptr)
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            _items.push_back({ addr, type, msg });
            endInsertRows();
        }
    }

    void clear() {
        beginResetModel();
        deleteItems();
        endResetModel();
    }

    void setHexView(bool on) {
        beginResetModel();
        _hexView = on;
        endResetModel();
    }

private:
    void deleteItems();

private:
    struct LogViewItem{
        quint16 Addr;
        QModbusDataUnit::RegisterType Type;
        const ModbusMessage* Msg;
    };

private:
    bool _hexView = false;
    QVector<LogViewItem> _items;
};

///
/// \brief The LogViewProxyModel class
///
class LogViewProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit LogViewProxyModel(QObject* parent = nullptr);

    void append(quint16 addr, QModbusDataUnit::RegisterType type, const ModbusMessage* msg) {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->append(addr, type, msg);
    }

    void clear() {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->clear();
    }

    void setHexView(bool on) {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->setHexView(on);
    }

    void setShowValid(bool on) {
        beginResetModel();
        _showValid = on;
        endResetModel();
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
    Q_OBJECT

public:
    explicit PdfExporter(QAbstractTableModel* model,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         QObject* parent = nullptr);
    void exportPdf(const QString& filename);

private:
    void calcTable(QPainter& painter);
    void paintPageHeader(int& yPos, QPainter& painter);
    void paintPageFooter(QPainter& painter);
    void paintTableHeader(int& yPos, QPainter& painter);
    void paintTableRow(int& yPos, QPainter& painter, int row);
    void paintTable(int& yPos, QPainter& painter);
    void paintVLine(int top, int bottom, QPainter& painter);

private:
    int _rowHeight = 0;
    int _colWidth = 65;
    int _headerWidth = 0;
    int _pageNumber = 1;
    const int _cy = 4;
    const int _cx = 10;
    QRect _pageRect;
    QAbstractTableModel* _model;
    const QString _startAddress;
    const QString _length;
    const QString _deviceId;
    const QString _pointType;
    const QString _regsOnQuery;
    QSharedPointer<QPrinter> _printer;
};

///
/// \brief The CsvExporter class
///
class CsvExporter: public QObject
{
    Q_OBJECT

public:
    explicit CsvExporter(QAbstractTableModel* model,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         QObject* parent = nullptr);
    void exportCsv(const QString& filename);

private:
    QAbstractTableModel* _model;
    const QString _startAddress;
    const QString _length;
    const QString _deviceId;
    const QString _pointType;
    const QString _regsOnQuery;
};

///
/// \brief The DialogAddressScan class
///
class DialogAddressScan : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddressScan(const DisplayDefinition& dd, DataDisplayMode mode, ByteOrder order, ModbusClient& client, QWidget *parent = nullptr);
    ~DialogAddressScan();

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_awake();
    void on_timeout();
    void on_modbusReply(QModbusReply* reply);
    void on_modbusRequest(int requestId, int deviceId, const QModbusRequest& data);
    void on_checkBoxHexView_toggled(bool);
    void on_checkBoxShowValid_toggled(bool);
    void on_comboBoxByteOrder_byteOrderChanged(ByteOrder);
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
    void updateTableView(int pointAddress, QVector<quint16> values);

    void updateLogView(int deviceId, const QModbusRequest& request);
    void updateLogView(const QModbusReply* reply);

    void exportPdf(const QString& filename);
    void exportCsv(const QString& filename);

private:
    Ui::DialogAddressScan *ui;

private:
    int _requestCount = 0;
    bool _scanning = false;
    bool _finished = false;
    quint64 _scanTime = 0;
    QTimer _scanTimer;
    ModbusClient& _modbusClient;
    QSharedPointer<TableViewItemModel> _viewModel;
};

#endif // DIALOGADDRESSSCAN_H
