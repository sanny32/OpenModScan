#ifndef POINTTYPECOMBOBOX_H
#define POINTTYPECOMBOBOX_H

#include <QComboBox>
#include <QModbusDataUnit>

///
/// \brief The PointTypeComboBox class
///
class PointTypeComboBox : public QComboBox
{
    Q_OBJECT
public:
    PointTypeComboBox(QWidget *parent = nullptr);

    QModbusDataUnit::RegisterType currentPointType() const;
    void setCurrentPointType(QModbusDataUnit::RegisterType pointType);
};

#endif // POINTTYPECOMBOBOX_H
