#ifndef DATASIMULATOR_H
#define DATASIMULATOR_H

#include <QTimer>
#include <QModbusDataUnit>
#include "modbussimulationparams.h"

class FormModSca;

///
/// \brief The DataSimulator class
///
class DataSimulator : public QObject
{
    Q_OBJECT
public:
    explicit DataSimulator(FormModSca* form);
    ~DataSimulator() override;

    void startSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const ModbusSimulationParams& params);
    void stopSimulation(QModbusDataUnit::RegisterType type, quint16 addr);
    void stopSimulations();

signals:
    void dataSimulated(QModbusDataUnit::RegisterType type, quint16 addr, QVariant value);

private slots:
    void on_timeout();

private:
    void initializeValue(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, double value);
    void randomSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const RandomSimulationParams& params);
    void incrementSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const IncrementSimulationParams& params);
    void decrementSimailation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const DecrementSimulationParams& params);
    void toggleSimulation(QModbusDataUnit::RegisterType type, quint16 addr);

private:
    FormModSca* _form;
    QTimer _timer;
    quint32 _elapsed;
    QMap<QPair<QModbusDataUnit::RegisterType, quint16>,
               QPair<DataDisplayMode, ModbusSimulationParams>> _simulationMap;
};

#endif // DATASIMULATOR_H
