#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include "enums.h"
#include "displaydefinition.h"

namespace Ui {
class FormModSca;
}

///
/// \brief The FormModSca class
///
class FormModSca : public QWidget
{
    Q_OBJECT

public:
    explicit FormModSca(int num, QWidget *parent = nullptr);
    ~FormModSca();

    DisplayDefinition displayDefinition() const;
    void setDisplayDefinition(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

private slots:
    void on_comboBoxModbusPointType_currentTextChanged(const QString&);

private:
    Ui::FormModSca *ui;

private:
    uint _scanRate = 1000;
};

#endif // FORMMODSCA_H
