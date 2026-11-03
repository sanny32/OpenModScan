#ifndef QINT64VALIDATOR_H
#define QINT64VALIDATOR_H

#include <QValidator>
#include <QObject>

///
/// \brief The QInt64Validator class
///
class QInt64Validator : public QValidator
{
    Q_OBJECT
public:
    explicit QInt64Validator(QObject *parent = nullptr);
    QInt64Validator(qint64 bottom, qint64 top, QObject *parent = nullptr);

    State validate(QString &, int &) const override;

private:
    qint64 _bottom;
    qint64 _top;
};

#endif // QINT64VALIDATOR_H
