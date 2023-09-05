#ifndef FORMATTEDSPINBOX_H
#define FORMATTEDSPINBOX_H

#include <QSpinBox>
#include <QObject>

class FormattedSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    FormattedSpinBox(QWidget* parent = nullptr);

protected:
    QString textFromValue(int val) const override;
};

#endif // FORMATTEDSPINBOX_H
