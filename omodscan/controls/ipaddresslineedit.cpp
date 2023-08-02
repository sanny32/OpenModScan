#include <QRegularExpressionValidator>
#include "ipaddresslineedit.h"

///
/// \brief IpAddressLineEdit::IpAddressLineEdit
/// \param parent
///
IpAddressLineEdit::IpAddressLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    const QString range = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    const QRegularExpression reg ("^" + range + "(\\." + range + ")" + "(\\." + range + ")" + "(\\." + range + ")$");
    setValidator(new QRegularExpressionValidator(reg, this));
}

///
/// \brief IpAddressLineEdit::value
/// \return
///
QHostAddress IpAddressLineEdit::value() const
{
    return QHostAddress(text());
}

///
/// \brief IpAddressLineEdit::setValue
/// \param address
///
void IpAddressLineEdit::setValue(const QHostAddress& address)
{
    setText(address.toString());
    emit editingFinished();
}
