#ifndef BYTEORDERCOMBOBOX_H
#define BYTEORDERCOMBOBOX_H

#include <QComboBox>
#include "enums.h"

///
/// \brief The ByteOrderComboBox class
///
class ByteOrderComboBox : public QComboBox
{
    Q_OBJECT
public:
    ByteOrderComboBox(QWidget *parent = nullptr);

    ByteOrder currentByteOrder() const;
    void setCurrentByteOrder(ByteOrder order);

signals:
    void byteOrderChanged(ByteOrder order);

private slots:
    void on_currentIndexChanged(int);
};

#endif // BYTEORDERCOMBOBOX_H
