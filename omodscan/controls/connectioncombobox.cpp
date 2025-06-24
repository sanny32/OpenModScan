#include <algorithm>
#include "serialportutils.h"
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
    setExcludeVirtuals(false);
}

///
/// \brief ConnectionComboBox::excludeVirtuals
/// \return
///
bool ConnectionComboBox::excludeVirtuals() const
{
    return _excludeVirtuals;
}

///
/// \brief ConnectionComboBox::setExcludeVirtuals
/// \param exclude
///
void ConnectionComboBox::setExcludeVirtuals(bool exclude)
{
    _excludeVirtuals = exclude;

    clear();

    addItem(tr("Remote TCP/IP Server"), ConnectionType::Tcp, QString());
    for(auto&& port: getAvailableSerialPorts(_excludeVirtuals))
    {
        const auto text = QString(tr("Direct Connection to %1")).arg(port);
        addItem(text, ConnectionType::Serial, port);
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
