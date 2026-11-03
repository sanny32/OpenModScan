#ifndef ADDRESSSPACECOMBOBOX_H
#define ADDRESSSPACECOMBOBOX_H

#include <QComboBox>
#include "enums.h"

///
/// \brief The AddressSpaceComboBox class
///
class AddressSpaceComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit AddressSpaceComboBox(QWidget *parent = nullptr);

    AddressSpace currentAddressSpace() const;
    void setCurrentAddressSpace(AddressSpace space);

signals:
    void addressSpaceChanged(AddressSpace space);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_currentIndexChanged(int);
};

#endif // ADDRESSSPACECOMBOBOX_H
