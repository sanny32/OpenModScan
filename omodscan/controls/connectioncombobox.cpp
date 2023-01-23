#include <algorithm>
#include <QSerialPortInfo>
#include "connectioncombobox.h"

///
/// \brief The ConnectionPort struct
///
struct ConnectionPort
{
    ConnectionType Type;
    QString PortName;
};
Q_DECLARE_METATYPE(ConnectionPort)

///
/// \brief ConnectionComboBox::ConnectionComboBox
/// \param parent
///
ConnectionComboBox::ConnectionComboBox(QWidget* parent)
    :QComboBox(parent)
{
    addItem("Remote TCP/IP Server", ConnectionType::Tcp, QString());

    QStringList ports;
    for(auto&& port: QSerialPortInfo::availablePorts())
    {
        const auto text = QString("Direct Connection to %1").arg(port.portName());
        addItem(text, ConnectionType::Serial, port.portName());
    }
}

///
/// \brief ConnectionComboBox::currentConnectionType
/// \return
///
ConnectionType ConnectionComboBox::currentConnectionType() const
{
    return currentData().value<ConnectionPort>().Type;
}

///
/// \brief ConnectionComboBox::setCurrentConnectionType
/// \param type
/// \param portName
///
void ConnectionComboBox::setCurrentConnectionType(ConnectionType type, const QString& portName)
{
    for(int  i = 0; i < count(); i++)
    {
        const auto cp = itemData(i).value<ConnectionPort>();
        if(cp.Type == type && cp.PortName == portName)
        {
            setCurrentIndex(i);
            break;
        }
    }
}

///
/// \brief ConnectionComboBox::currentPortName
/// \return
///
QString ConnectionComboBox::currentPortName() const
{
    return currentData().value<ConnectionPort>().PortName;
}

///
/// \brief ConnectionComboBox::addItem
/// \param text
/// \param type
/// \param portName
///
void ConnectionComboBox::addItem(const QString& text, ConnectionType type, const QString& portName)
{
    ConnectionPort cp;
    cp.Type = type;
    cp.PortName = portName;
    QComboBox::addItem(text, QVariant::fromValue(cp));
}
