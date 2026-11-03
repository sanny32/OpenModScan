#ifndef DIALOGAUTOSIMULATION_H
#define DIALOGAUTOSIMULATION_H

#include "qfixedsizedialog.h"
#include "modbussimulationparams.h"

namespace Ui {
class DialogAutoSimulation;
}

class DialogAutoSimulation : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogAutoSimulation(DataDisplayMode mode, ModbusSimulationParams& params, QWidget *parent = nullptr);
    ~DialogAutoSimulation();

    void accept() override;

private slots:
    void on_checkBoxEnabled_toggled();
    void on_comboBoxSimulationType_currentIndexChanged(int);
    void on_lineEditLowLimit_valueChanged(const QVariant&);
    void on_lineEditHighLimit_valueChanged(const QVariant&);

private:
    void updateLimits();

private:
    Ui::DialogAutoSimulation *ui;

private:
    ModbusSimulationParams& _params;
    DataDisplayMode _displayMode;
};

#endif // DIALOGAUTOSIMULATION_H
