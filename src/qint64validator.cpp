#include "qint64validator.h"

///
/// \brief QInt64Validator::QInt64Validator
/// \param parent
///
QInt64Validator::QInt64Validator(QObject *parent)
    : QValidator{parent}
    ,_bottom(0)
    ,_top(UINT_MAX)
{
}

///
/// \brief QInt64Validator::QInt64Validator
/// \param bottom
/// \param top
/// \param parent
///
QInt64Validator::QInt64Validator(qint64 bottom, qint64 top, QObject *parent)
    : QValidator(parent)
    ,_bottom(bottom)
    ,_top(top)
{

}

///
/// \brief QInt64Validator::validate
/// \return
///
QValidator::State QInt64Validator::validate(QString& input, int &) const
{
    bool ok = false;
    const auto value = input.toLongLong(&ok, 10);

    if (ok && value >=_bottom && value <=_top )
    {
        return QValidator::Acceptable;
    }

    return QValidator::Invalid;
}
