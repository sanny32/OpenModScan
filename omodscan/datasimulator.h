#ifndef DATASIMULATOR_H
#define DATASIMULATOR_H

#include <QTimer>
#include <QModbusDataUnit>
#include "modbussimulationparams.h"

typedef QMap<QPair<QModbusDataUnit::RegisterType, quint16>, ModbusSimulationParams> ModbusSimulationMap;

///
/// \brief The DataSimulator class
///
class DataSimulator : public QObject
{
    Q_OBJECT

public:
    explicit DataSimulator(QObject* parent);
    ~DataSimulator() override;

    void startSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const ModbusSimulationParams& params);
    void stopSimulation(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);
    void stopSimulations();

    void pauseSimulations();
    void resumeSimulations();
    void restartSimulations();

    ModbusSimulationParams simulationParams(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId) const;
    ModbusSimulationMap simulationMap(quint8 deviceId) const;

signals:
    void simulationStarted(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);
    void simulationStopped(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);
    void dataSimulated(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, QVariant value);

private slots:
    void on_timeout();

private:
    void randomSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const RandomSimulationParams& params);
    void incrementSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const IncrementSimulationParams& params);
    void decrementSimailation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const DecrementSimulationParams& params);
    void toggleSimulation(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);

private:
    QTimer _timer;
    quint32 _elapsed;
    const int _interval = 1000;

    struct SimulationParams {
        DataDisplayMode Mode;
        ModbusSimulationParams Params;
        QVariant CurrentValue;
    };
    struct SimulationKey{
        QModbusDataUnit::RegisterType Type;
        quint16 Address;
        quint8 DeviceId;
        bool operator<(const SimulationKey& key) const{
            return Type < key.Type && Address < key.Address && DeviceId < key.DeviceId;
        }
    };

    QMap<SimulationKey, SimulationParams> _simulationMap;
};

#endif // DATASIMULATOR_H
