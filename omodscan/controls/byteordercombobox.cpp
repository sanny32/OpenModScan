#include <QEvent>
#include "byteordercombobox.h"

///
/// \brief ByteOrderComboBox::ByteOrderComboBox
/// \param parent
///
ByteOrderComboBox::ByteOrderComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem(tr("Direct"), QVariant::fromValue(ByteOrder::Direct));
    addItem(tr("Swapped"), QVariant::fromValue(ByteOrder::Swapped));

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ByteOrderComboBox::on_currentIndexChanged);
}

///
/// \brief ByteOrderComboBox::changeEvent
/// \param event
///
void ByteOrderComboBox::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        setItemText(0, tr("Direct"));
        setItemText(1, tr("Swapped"));
    }

    QComboBox::changeEvent(event);
}

///
/// \brief ByteOrderComboBox::currentByteOrder
/// \return
///
ByteOrder ByteOrderComboBox::currentByteOrder() const
{
    return currentData().value<ByteOrder>();
}

///
/// \brief ByteOrderComboBox::setCurrentByteOrder
/// \param order
///
void ByteOrderComboBox::setCurrentByteOrder(ByteOrder order)
{
    const auto idx = findData(QVariant::fromValue(order));
    setCurrentIndex(idx);
}

///
/// \brief ByteOrderComboBox::on_currentIndexChanged
/// \param index
///
void ByteOrderComboBox::on_currentIndexChanged(int index)
{
    emit byteOrderChanged(itemData(index).value<ByteOrder>());
}
