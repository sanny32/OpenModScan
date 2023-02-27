#include "byteordercombobox.h"

///
/// \brief ByteOrderComboBox::ByteOrderComboBox
/// \param parent
///
ByteOrderComboBox::ByteOrderComboBox(QWidget *parent)
    :QComboBox(parent)
{
    addItem(tr("Little Endian"), QVariant::fromValue(ByteOrder::LittleEndian));
    addItem(tr("Big Endian"), QVariant::fromValue(ByteOrder::BigEndian));

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
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
