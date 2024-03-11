#ifndef QUINTVALIDATOR_H
#define QUINTVALIDATOR_H

#include <QValidator>
#include <QObject>

///
/// \brief The QUIntValidator class
///
class QUIntValidator : public QValidator
{
    Q_OBJECT
public:
    explicit QUIntValidator(QObject *parent = nullptr);
    QUIntValidator(quint64 bottom, quint64 top, QObject *parent = nullptr);

    State validate(QString &, int &) const override;

private:
    quint64 _bottom;
    quint64 _top;
};

#endif // QUINTVALIDATOR_H
