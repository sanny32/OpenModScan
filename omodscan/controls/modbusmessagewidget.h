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

    const ModbusMessage* modbusMessage() const;
    void setModbusMessage(const ModbusMessage* msg);

    bool showTimestamp() const;
    void setShowTimestamp(bool on);

private:
    void update();

private:
    ByteOrder _byteOrder;
    DataDisplayMode _dataDisplayMode;
    bool _showTimestamp;
    const ModbusMessage* _msg;
};

#endif // MODBUSMESSAGEWIDGET_H
