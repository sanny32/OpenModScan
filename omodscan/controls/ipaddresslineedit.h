#ifndef IPADDRESSLINEEDIT_H
#define IPADDRESSLINEEDIT_H

#include <QLineEdit>
#include <QHostAddress>

///
/// \brief The IpAddressLineEdit class
///
class IpAddressLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit IpAddressLineEdit(QWidget* parent = nullptr);

    QHostAddress value() const;
    void setValue(const QHostAddress& address);
    void setValue(quint32 address);
};

#endif // IPADDRESSLINEEDIT_H
