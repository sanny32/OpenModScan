#ifndef QHEXVALIDATOR_H
#define QHEXVALIDATOR_H

#include <QValidator>

///
/// \brief The QHexValidator class
///
class QHexValidator : public QIntValidator
{
public:
    explicit QHexValidator(QObject *parent = nullptr);
    QHexValidator(int bottom, int top, QObject* parent = nullptr);

    State validate(QString &, int &) const override;
};

#endif // QHEXVALIDATOR_H
