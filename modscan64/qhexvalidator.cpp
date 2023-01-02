#include "qhexvalidator.h"

///
/// \brief QHexValidator::QHexValidator
/// \param parent
///
QHexValidator::QHexValidator(QObject *parent)
    : QIntValidator{parent}
{
}

QHexValidator::QHexValidator(int bottom, int top, QObject* parent)
    : QIntValidator(bottom, top, parent)
{
}

///
/// \brief QHexValidator::validate
/// \param input
/// \return
///
QHexValidator::State QHexValidator::validate(QString& input, int&) const
{
    QRegExp rx("(?:[0-9a-fA-F]{2})*[0-9a-fA-F]{0,3}");
    if (rx.exactMatch(input))
    {
        return QValidator::Acceptable;
    }

    return QValidator::Invalid;
}
