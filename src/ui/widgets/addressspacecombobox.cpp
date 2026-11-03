#include <QEvent>
#include "addressspacecombobox.h"

///
/// \brief AddressSpaceComboBox::AddressSpaceComboBox
/// \param parent
///
AddressSpaceComboBox::AddressSpaceComboBox(QWidget* parent)
    : QComboBox(parent)
{
    addItem(tr("5-digits"), QVariant::fromValue(AddressSpace::Addr5Digits));
    addItem(tr("6-digits"), QVariant::fromValue(AddressSpace::Addr6Digits));

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddressSpaceComboBox::on_currentIndexChanged);
}

///
/// \brief AddressSpaceComboBox::changeEvent
/// \param event
///
void AddressSpaceComboBox::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        for(int i = 0; i < count(); i++)
        {
            switch(itemData(i).value<AddressSpace>())
            {
                case AddressSpace::Addr5Digits:
                    setItemText(i, tr("5-digits"));
                break;

                case AddressSpace::Addr6Digits:
                    setItemText(i, tr("6-digits"));
                break;
            }
        }
    }

    QComboBox::changeEvent(event);
}

///
/// \brief AddressSpaceComboBox::currentAddressSpace
/// \return
///
AddressSpace AddressSpaceComboBox::currentAddressSpace() const
{
    return currentData().value<AddressSpace>();
}

///
/// \brief AddressSpaceComboBox::setCurrentAddressSpace
/// \param space
///
void AddressSpaceComboBox::setCurrentAddressSpace(AddressSpace space)
{
    const auto idx = findData(QVariant::fromValue(space));
    if(idx == currentIndex())
    {
        emit currentIndexChanged(idx);
    }
    else if(idx != -1)
    {
        setCurrentIndex(idx);
    }
}

///
/// \brief AddressSpaceComboBox::on_currentIndexChanged
/// \param index
///
void AddressSpaceComboBox::on_currentIndexChanged(int index)
{
    emit addressSpaceChanged(itemData(index).value<AddressSpace>());
}
