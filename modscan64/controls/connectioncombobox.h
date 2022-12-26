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

    ConnectionType currentConnectionType() const;
    QString currentPortName() const;

private:
    void addItem(const QString& text, ConnectionType type, const QString& portName);
};

#endif // CONNECTIONCOMBOBOX_H
