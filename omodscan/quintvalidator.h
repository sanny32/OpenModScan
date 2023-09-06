#ifndef QUINTVALIDATOR_H
#define QUINTVALIDATOR_H

#include <QValidator>
#include <QObject>

class QUIntValidator : public QValidator
{
    Q_OBJECT
public:
    explicit QUIntValidator(QObject *parent = nullptr);
    QUIntValidator(uint bottom, uint top, QObject *parent = nullptr);

    State validate(QString &, int &) const override;

private:
    uint _bottom;
    uint _top;
};

#endif // QUINTVALIDATOR_H
