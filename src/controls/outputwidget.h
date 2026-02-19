#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QFile>
#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include <QListWidgetItem>
#include <QModbusReply>
#include <QPainter>
#include <QStyledItemDelegate>
#include "enums.h"
#include "modbusmessage.h"
#include "datasimulator.h"
#include "displaydefinition.h"

namespace Ui {
class OutputWidget;
}

class OutputWidget;

///
/// \brief The QEmptyPixmap class
///
class QEmptyPixmap : public QPixmap {
public:
    QEmptyPixmap(const QSize& size) :
        QPixmap(size) {
        fill(Qt::transparent);
    }
};

///
/// \brief The ItemMapKey class
///
struct ItemMapKey {
    quint8 DeviceId;
    QModbusDataUnit::RegisterType Type;
    quint16 Address;

    bool operator<(const  ItemMapKey &other) const {
        if (DeviceId != other.DeviceId)
            return DeviceId < other.DeviceId;
        if (Type != other.Type)
            return Type < other.Type;
        return Address < other.Address;
    }
};

typedef QMap<ItemMapKey, QColor> AddressColorMap2;
typedef QMap<QPair<QModbusDataUnit::RegisterType, quint16>, QColor> AddressColorMap;

typedef QMap<ItemMapKey, QString> AddressDescriptionMap2;
typedef QMap<QPair<QModbusDataUnit::RegisterType, quint16>, QString> AddressDescriptionMap;

///
/// \brief The OutputListModel class
///
class OutputListModel : public QAbstractListModel
{
    Q_OBJECT

    friend class OutputWidget;

public:
    enum SimulationIconType
    {
        SimulationIconNone,
        SimulationIcon16Bit,
        SimulationIcon32Bit,
        SimulationIcon64Bit
    };
    Q_ENUM(SimulationIconType)

    explicit OutputListModel(OutputWidget* parent);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool isValid() const;
    QVector<quint16> values() const;

    void clear();
    void update();
    void updateData(const QModbusDataUnit& data);

    int columnsDistance() const {
        return _columnsDistance;
    }
    void setColumnsDistance(int value) {
        _columnsDistance = qMax(1, value);
    }

    QModelIndex find(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr) const;

private:
    SimulationIconType simulationIcon(int row) const;

private:
    struct ItemData
    {
        quint32 Address = 0;
        QVariant Value;
        QString ValueStr;
        QString Description;
        bool Simulated = false;
        SimulationIconType SimulationIcon = SimulationIconNone;
        QColor BgColor;
        QColor FgColor;
    };

    OutputWidget* _parentWidget;
    QModbusDataUnit _lastData;
    const QPixmap _iconSimulation16Bit;
    const QPixmap _iconSimulation32Bit;
    const QPixmap _iconSimulation64Bit;
    const QEmptyPixmap _iconSimulationOff;
    int _columnsDistance = 16;
    QMap<int, ItemData> _mapItems;
};

///
/// \brief The OutputWidget class
///
class OutputWidget : public QWidget
{
    Q_OBJECT

    friend class OutputListModel;

public:  
    explicit OutputWidget(QWidget *parent = nullptr);
    ~OutputWidget() override;

    QVector<quint16> data() const;

    void setup(const DisplayDefinition& dd, ModbusMessage::ProtocolType protocol, const ModbusSimulationMap2& simulations);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    ModbusMessage::ProtocolType protocol() const;
    void setProtocol(ModbusMessage::ProtocolType type);

    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder order);

    QString codepage() const;
    void setCodepage(const QString& name);

    bool displayHexAddresses() const;
    void setDisplayHexAddresses(bool on);

    CaptureMode captureMode() const;
    void startTextCapture(const QString& file);
    void stopTextCapture();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& clr);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor& clr);

    QColor statusColor() const;
    void setStatusColor(const QColor& clr);

    QFont font() const;
    void setFont(const QFont& font);

    int zoomPercent() const;
    void setZoomPercent(int zoomPercent);

    int dataViewColumnsDistance() const;
    void setDataViewColumnsDistance(int value);

    int logViewLimit() const;
    void setLogViewLimit(int l);

    bool autoscrollLogView() const;
    void setAutosctollLogView(bool on);

    void setStatus(const QString& status);

    void paint(const QRect& rc, QPainter& painter);

    void updateTraffic(QSharedPointer<const ModbusMessage> msg);
    void updateData(const QModbusDataUnit& data);

    AddressColorMap2 colorMap() const;
    void setColor(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const QColor& clr);

    AddressDescriptionMap2 descriptionMap() const;
    void setDescription(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const QString& desc);

    void setSimulated(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, bool on);

public slots:
    void clearLogView();

signals:
    bool canWriteValue();
    void startTextCaptureError(const QString& error);
    void itemDoubleClicked(quint16 address, const QVariant& value);

protected:
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void on_listView_doubleClicked(const QModelIndex& index);
    void on_listView_customContextMenuRequested(const QPoint &pos);

private:
    void setUninitializedStatus();
    void captureString(const QString& s);
    void showModbusMessage(const QModelIndex& index);
    void hideModbusMessage();
    void showZoomOverlay();
    void updateLogView(QSharedPointer<const ModbusMessage> msg);
    QModelIndex getValueIndex(const QModelIndex& index) const;

private:
    Ui::OutputWidget *ui;
    QLabel* _zoomLabel = nullptr;
    QTimer* _zoomHideTimer = nullptr;

private:
    qreal _baseFontSize = 0.0;
    int _zoomPercent = 100;

    bool _displayHexAddresses;
    DisplayMode _displayMode;
    DataDisplayMode _dataDisplayMode;
    ModbusMessage::ProtocolType _protocol;
    ByteOrder _byteOrder;
    QString _codepage;
    DisplayDefinition _displayDefinition;
    QFile _fileCapture;
    AddressColorMap2 _colorMap;
    AddressDescriptionMap2 _descriptionMap;
    QSharedPointer<OutputListModel> _listModel;
};

///
/// \brief operator <<
/// \param out
/// \param key
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const ItemMapKey& key)
{
    out << key.DeviceId;
    out << key.Type;
    out << key.Address;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, ItemMapKey& key)
{
    in >> key.DeviceId;
    in >> key.Type;
    in >> key.Address;
    return in;
}

#endif // OUTPUTWIDGET_H
