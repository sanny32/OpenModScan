#ifndef MODBUSMESSAGEWIDGET_H
#define MODBUSMESSAGEWIDGET_H

#include <QListWidget>
#include "modbusmessage.h"

///
/// \brief The ModbusMessageWidget class
///
class ModbusMessageWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ModbusMessageWidget(QWidget *parent = nullptr);

    void clear();

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder order);

    QSharedPointer<const ModbusMessage> modbusMessage() const;
    void setModbusMessage(QSharedPointer<const ModbusMessage> msg);

    bool showTimestamp() const;
    void setShowTimestamp(bool on);

    void setStatusColor(const QColor& clr);

protected:
    void changeEvent(QEvent* event) override;

private:
    void update();

private:
    QColor _statusClr;
    ByteOrder _byteOrder;
    DataDisplayMode _dataDisplayMode;
    bool _showTimestamp;
    QSharedPointer<const ModbusMessage> _mm;
};

#endif // MODBUSMESSAGEWIDGET_H
