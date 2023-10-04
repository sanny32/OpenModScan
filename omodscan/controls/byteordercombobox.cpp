#include "byteordercombobox.h"

///
/// \brief ByteOrderComboBox::ByteOrderComboBox
/// \param parent
///
ByteOrderComboBox::ByteOrderComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem("Little-Endian", QVariant::fromValue(ByteOrder::LittleEndian));
    addItem("Big-Endian", QVariant::fromValue(ByteOrder::BigEndian));

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ByteOrderComboBox::on_currentIndexChanged);
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
