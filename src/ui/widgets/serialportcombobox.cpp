#include <algorithm>
#include "serialportutils.h"
#include "serialportcombobox.h"

///
/// \brief SerialPortComboBox::SerialPortComboBox
/// \param parent
///
SerialPortComboBox::SerialPortComboBox(QWidget* parent)
    :QComboBox(parent)
{
    setExcludeVirtuals(false);
}

///
/// \brief SerialPortComboBox::excludeVirtuals
/// \return
///
bool SerialPortComboBox::excludeVirtuals() const
{
    return _excludeVirtuals;
}

///
/// \brief SerialPortComboBox::setExcludeVirtuals
/// \param exclude
///
void SerialPortComboBox::setExcludeVirtuals(bool exclude)
{
    _excludeVirtuals = exclude;

    clear();
    for(auto&& port: getAvailableSerialPorts(_excludeVirtuals)) {
        addItem(port);
    }
}

///
/// \brief SerialPortComboBox::currentPortName
/// \return
///
QString SerialPortComboBox::currentPortName() const
{
    return currentText();
}

