#include <QEvent>
#include "addressbasecombobox.h"

///
/// \brief AddressBaseComboBox::AddressBaseComboBox
/// \param parent
///
AddressBaseComboBox::AddressBaseComboBox(QWidget* parent)
    : QComboBox(parent)
{
    addItem(tr("0-based"), QVariant::fromValue(AddressBase::Base0));
    addItem(tr("1-based"), QVariant::fromValue(AddressBase::Base1));

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddressBaseComboBox::on_currentIndexChanged);
}

///
/// \brief AddressBaseComboBox::changeEvent
/// \param event
///
void AddressBaseComboBox::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        for(int i = 0; i < count(); i++)
        {
            switch(itemData(i).value<AddressBase>())
            {
                case AddressBase::Base0:
                    setItemText(i, tr("0-based"));
                break;

                case AddressBase::Base1:
                    setItemText(i, tr("1-based"));
                break;
            }
        }
    }

    QComboBox::changeEvent(event);
}

///
/// \brief AddressBaseComboBox::currentAddressBase
/// \return
///
AddressBase AddressBaseComboBox::currentAddressBase() const
{
    return currentData().value<AddressBase>();
}

///
/// \brief AddressBaseComboBox::setCurrentAddressBase
/// \param pointType
///
void AddressBaseComboBox::setCurrentAddressBase(AddressBase base)
{
    const auto idx = findData(QVariant::fromValue(base));
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
/// \brief AddressBaseComboBox::on_currentIndexChanged
/// \param index
///
void AddressBaseComboBox::on_currentIndexChanged(int index)
{
    emit addressBaseChanged(itemData(index).value<AddressBase>());
}
