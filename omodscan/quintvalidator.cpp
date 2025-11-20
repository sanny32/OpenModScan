#include "quintvalidator.h"

///
/// \brief QUIntValidator::QUIntValidator
/// \param parent
///
QUIntValidator::QUIntValidator(QObject *parent)
    : QValidator{parent}
    ,_bottom(0)
    ,_top(UINT_MAX)
{
}

///
/// \brief QUIntValidator::QUIntValidator
/// \param bottom
/// \param top
/// \param parent
///
QUIntValidator::QUIntValidator(quint64 bottom, quint64 top, QObject *parent)
    : QValidator(parent)
    ,_bottom(bottom)
    ,_top(top)
{

}

///
/// \brief QUIntValidator::validate
/// \return
///
QValidator::State QUIntValidator::validate(QString& input, int &) const
{
    if(input.isEmpty())
        return QValidator::Intermediate;

    bool ok = false;
    const auto value = input.toULongLong(&ok, 10);

    if (ok && value >=_bottom && value <=_top )
    {
        return QValidator::Acceptable;
    }

    return QValidator::Invalid;
}

///
/// \brief QUIntValidator::fixup
/// \param input
///
void QUIntValidator::fixup(QString& input) const
{
    QValidator::fixup(input);

    if(input.isEmpty()) {
        input = QString::number(_bottom);
    }
}
