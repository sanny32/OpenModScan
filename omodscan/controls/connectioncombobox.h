#ifndef CONNECTIONCOMBOBOX_H
#define CONNECTIONCOMBOBOX_H

#include <QComboBox>
#include "enums.h"

///
/// \brief The ConnectionComboBox class
///
class ConnectionComboBox : public QComboBox
{
    Q_OBJECT
public:
    ConnectionComboBox(QWidget* parent = nullptr);

    bool excludeVirtuals() const;
    void setExcludeVirtuals(bool exclude);

    ConnectionType currentConnectionType() const;
    void setCurrentConnectionType(ConnectionType type, const QString& portName);

    QString currentPortName() const;

private:
    void addItem(const QString& text, ConnectionType type, const QString& portName);

private:
    bool _excludeVirtuals = false;
};

#endif // CONNECTIONCOMBOBOX_H
