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
    bool ok = false;
    input.toInt(&ok, 16);

    if (ok || input.isEmpty())
    {
        return QValidator::Acceptable;
    }

    return QValidator::Invalid;
}
