#ifndef IPADDRESSLINEEDIT_H
#define IPADDRESSLINEEDIT_H

#include <QLineEdit>

///
/// \brief The IpAddressLineEdit class
///
class IpAddressLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit IpAddressLineEdit(QWidget* parent = nullptr);
};

#endif // IPADDRESSLINEEDIT_H
