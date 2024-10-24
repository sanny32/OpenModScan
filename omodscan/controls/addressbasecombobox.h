#ifndef ADDRESSBASECOMBOBOX_H
#define ADDRESSBASECOMBOBOX_H

#include <QComboBox>
#include "enums.h"

///
/// \brief The AddressBaseComboBox class
///
class AddressBaseComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit AddressBaseComboBox(QWidget *parent = nullptr);

    AddressBase currentAddressBase() const;
    void setCurrentAddressBase(AddressBase base);

signals:
    void addressBaseChanged(AddressBase base);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_currentIndexChanged(int);
};

#endif // ADDRESSBASECOMBOBOX_H
