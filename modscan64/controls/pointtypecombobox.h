#ifndef POINTTYPECOMBOBOX_H
#define POINTTYPECOMBOBOX_H

#include <QComboBox>

///
/// \brief The PointTypeComboBox class
///
class PointTypeComboBox : public QComboBox
{
    Q_OBJECT
public:
    PointTypeComboBox(QWidget *parent = nullptr);

    uint currentPointType() const;
    void setCurrentPointType(uint pointType);
};

#endif // POINTTYPECOMBOBOX_H
