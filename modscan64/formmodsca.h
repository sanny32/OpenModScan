#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
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
    void on_timeout();
    void on_comboBoxModbusPointType_currentTextChanged(const QString&);

private:
    Ui::FormModSca *ui;
    QTimer _timer;
};

#endif // FORMMODSCA_H
