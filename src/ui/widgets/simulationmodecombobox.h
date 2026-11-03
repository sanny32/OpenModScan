#ifndef SIMULATIONMODECOMBOBOX_H
#define SIMULATIONMODECOMBOBOX_H

#include <QComboBox>
#include <QModbusDataUnit>
#include "enums.h"

class SimulationModeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SimulationModeComboBox(QWidget *parent = nullptr);

    SimulationMode currentSimulationMode() const;
    void setCurrentSimulationMode(SimulationMode mode);

    void setup(QModbusDataUnit::RegisterType type);

signals:
    void simulationModeChanged(SimulationMode value);

private slots:
    void on_currentIndexChanged(int);
};

#endif // SIMULATIONMODECOMBOBOX_H
