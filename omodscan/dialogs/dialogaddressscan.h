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
    explicit TableViewItemModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    AddressBase addressBase() const;
    void setAddressBase(AddressBase base);

    AddressSpace addressSpace() const;
    void setAddressSpace(AddressSpace space);

    bool showLeadingZeros() const;
    void setShowLeadingZeros(bool value);

    void reset(const ModbusDataUnit& data, int columns = 10){
        beginResetModel();
        _columns = columns;
        _data = data;
        endResetModel();
    }

    void setHexView(bool on){
        beginResetModel();
        _hexView = on;
        endResetModel();
    }

    void setHexAddress(bool on) {
        beginResetModel();
        _hexAddress = on;
        endResetModel();
    }

    void setByteOrder(ByteOrder order){
        beginResetModel();
        _byteOrder = order;
        endResetModel();
    }

private:
    int getAddress(int idx) const;

private:
    int _columns = 10;
    ModbusDataUnit _data;
    bool _hexView = false;
    bool _hexAddress = false;
    AddressSpace _addressSpace = AddressSpace::Addr6Digits;
    AddressBase _addressBase = AddressBase::Base1;
    ByteOrder _byteOrder = ByteOrder::Direct;
    bool _showLeadingZeros = true;
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

    AddressBase addressBase() const;
    void setAddressBase(AddressBase base);

    AddressSpace addressSpace() const;
    void setAddressSpace(AddressSpace space);

    void append(quint16 addr, QModbusDataUnit::RegisterType type, QSharedPointer<const ModbusMessage> msg) {
        if(msg == nullptr) return;
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        _items.push_back({ addr, type, msg });
        endInsertRows();
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

    void setHexAddress(bool on) {
        beginResetModel();
        _hexAddress = on;
        endResetModel();
    }

private:
    void deleteItems();

private:
    struct LogViewItem{
        quint16 Addr;
        QModbusDataUnit::RegisterType Type;
        QSharedPointer<const ModbusMessage> Msg;
    };

private:
    bool _hexView = false;
    bool _hexAddress = false;
    AddressSpace _addressSpace = AddressSpace::Addr6Digits;
    AddressBase _addressBase = AddressBase::Base1;
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

    void append(quint16 addr, QModbusDataUnit::RegisterType type, QSharedPointer<const ModbusMessage> msg) {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->append(addr, type, msg);
    }

    void clear() {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->clear();
    }

    void setAddressBase(AddressBase base) {
        if(sourceModel())
            ((LogViewModel*)sourceModel())->setAddressBase(base);
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
    explicit PdfExporter(QAbstractItemModel* model,
                         const QString& addressBase,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         const QString& byteOrder,
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
    QAbstractItemModel* _model;
    const QString _addressBase;
    const QString _startAddress;
    const QString _length;
    const QString _deviceId;
    const QString _pointType;
    const QString _regsOnQuery;
    const QString _byteOrder;
    QSharedPointer<QPrinter> _printer;
};

///
/// \brief The CsvExporter class
///
class CsvExporter: public QObject
{
    Q_OBJECT

public:
    explicit CsvExporter(QAbstractItemModel* model,
                         const QString& addressBase,
                         const QString& startAddress,
                         const QString& length,
                         const QString& devId,
                         const QString& pointType,
                         const QString& regsOnQuery,
                         const QString& byteOrder,
                         QObject* parent = nullptr);
    void exportCsv(const QString& filename);

private:
    QAbstractItemModel* _model;
    const QString _addressBase;
    const QString _startAddress;
    const QString _length;
    const QString _deviceId;
    const QString _pointType;
    const QString _regsOnQuery;
    const QString _byteOrder;
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
    void on_modbusRequest(int requestId, int deviceId, int transactionId, const QModbusRequest& data);
    void on_checkBoxHexView_toggled(bool);
    void on_checkBoxShowValid_toggled(bool);
    void on_lineEditStartAddress_valueChanged(const QVariant& value);
    void on_lineEditLength_valueChanged(const QVariant& value);
    void on_lineEditToFind_valueChanged(const QVariant& value);
    void on_comboBoxPointType_pointTypeChanged(QModbusDataUnit::RegisterType pointType);
    void on_comboBoxAddressBase_addressBaseChanged(AddressBase base);
    void on_comboBoxByteOrder_byteOrderChanged(ByteOrder);
    void on_logView_clicked(const QModelIndex &index);
    void on_pushButtonScan_clicked();
    void on_pushButtonExport_clicked();
    void on_pushButtonFind_clicked();

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

    void updateLogView(int deviceId, int transactionId, const QModbusRequest& request);
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
};

#endif // DIALOGADDRESSSCAN_H
