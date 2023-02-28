#include "simulationmodecombobox.h"

///
/// \brief SimulationComboBox::SimulationComboBox
/// \param parent
///
SimulationModeComboBox::SimulationModeComboBox(QWidget *parent)
    : QComboBox(parent)
{

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
}

///
/// \brief SimulationModeComboBox::currentSimulationMode
/// \return
///
SimulationMode SimulationModeComboBox::currentSimulationMode() const
{
    return currentData().value<SimulationMode>();
}

///
/// \brief SimulationModeComboBox::setCurrentSimulationMode
/// \param mode
///
void SimulationModeComboBox::setCurrentSimulationMode(SimulationMode mode)
{
    const auto idx = findData(QVariant::fromValue(mode));
    setCurrentIndex(idx);
}

///
/// \brief SimulationModeComboBox::setup
/// \param type
///
void SimulationModeComboBox::setup(QModbusDataUnit::RegisterType type)
{
    switch(type)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            addItem("Random", QVariant::fromValue(SimulationMode::Random));
            addItem("Toggle", QVariant::fromValue(SimulationMode::Toggle));
        break;

        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            addItem("Random", QVariant::fromValue(SimulationMode::Random));
            addItem("Increment", QVariant::fromValue(SimulationMode::Increment));
            addItem("Decrement", QVariant::fromValue(SimulationMode::Decrement));
        break;

        default:
        break;
    }
}

///
/// \brief SimulationModeComboBox::on_currentIndexChanged
/// \param index
///
void SimulationModeComboBox::on_currentIndexChanged(int index)
{
    emit simulationModeChanged(itemData(index).value<SimulationMode>());
}
