#include <float.h>
#include "modbuslimits.h"
#include "numericutils.h"
#include "modbusclient.h"
#include "waitcursor.h"
#include "datasimulator.h"
#include "dialogautosimulation.h"
#include "dialogwriteholdingregister.h"
#include "ui_dialogwriteholdingregister.h"

///
/// \brief The SimButtonColors class
///
struct SimButtonColors
{
    QString base;
    QString hover;
    QString pressed;
    QString border;
};

///
/// \brief simColors
/// \param registersCount
/// \return
///
static SimButtonColors simColors(DataDisplayMode mode)
{
    switch(registersCount(mode))
    {
        case 2:
            return { "#5680D0", "#4E75C0", "#466AB0", "#3E5FA0" };

        case 4:
            return { "#D74D9D", "#C9458F", "#BB3D81", "#A73573" };

        default:
            return { "#4CAF50", "#45A049", "#3E8E41", "#3E8E41" };
    }
}

///
/// \brief DialogWriteHoldingRegister::DialogWriteHoldingRegister
/// \param params
/// \param hexAddress
/// \param dataSimulator
/// \param parent
///
DialogWriteHoldingRegister::DialogWriteHoldingRegister(ModbusWriteParams& params, bool hexAddress,
                                                       DataSimulator* dataSimulator, QWidget* parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogWriteHoldingRegister)
    ,_writeParams(params)
    ,_dataSimulator(dataSimulator)
{
    ui->setupUi(this);

    if(params.ForceModbus15And16Func) {
        setWindowTitle(tr("16: Write Holding Register"));
    }

    ui->lineEditNode->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditNode->setValue(params.DeviceId);

    ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditAddress->setValue(params.Address);

    if(_dataSimulator != nullptr)
    {
        const int simAddr = params.Address - (params.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(params.DeviceId, QModbusDataUnit::HoldingRegisters, simAddr);
        if(!_dataSimulator->canStartSimulation(params.DisplayMode, params.DeviceId, QModbusDataUnit::HoldingRegisters, simAddr)) {
            _simParams.Mode = SimulationMode::Disabled;
        }
    }
    updateSimulationButton();

    switch(params.DisplayMode)
    {
        case DataDisplayMode::UInt16:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::Int16:
            ui->lineEditValue->setInputRange(SHRT_MIN, SHRT_MAX);
            ui->lineEditValue->setValue(params.Value.toInt());
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Hex:
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
            ui->labelValue->setText(tr("Value, (HEX): "));
            ui->lineEditValue->setLeadingZeroes(true);
            ui->lineEditValue->setInputMode(NumericLineEdit::HexMode);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::Ansi:
            ui->labelValue->setText(tr("Value, (ANSI): "));
            ui->lineEditValue->setInputMode(NumericLineEdit::AnsiMode);
            ui->lineEditValue->setCodepage(params.Codepage);
            ui->lineEditValue->setValue(params.Value.toUInt());
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditValue->setInputMode(NumericLineEdit::FloatMode);
            ui->lineEditValue->setValue(params.Value.toFloat());
            ui->controlBitPattern->setEnabled(false);
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditValue->setInputMode(NumericLineEdit::DoubleMode);
            ui->lineEditValue->setValue(params.Value.toDouble());
            ui->controlBitPattern->setEnabled(false);
        break;

        case DataDisplayMode::Int32:
        case DataDisplayMode::SwappedInt32:
            ui->lineEditValue->setInputMode(NumericLineEdit::Int32Mode);
            ui->lineEditValue->setValue(params.Value.toInt());
            ui->controlBitPattern->setEnabled(false);
        break;

        case DataDisplayMode::UInt32:
        case DataDisplayMode::SwappedUInt32:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputMode(NumericLineEdit::UInt32Mode);
            ui->lineEditValue->setValue(params.Value.toUInt());
            ui->controlBitPattern->setEnabled(false);
        break;

        case DataDisplayMode::Int64:
        case DataDisplayMode::SwappedInt64:
            ui->lineEditValue->setInputMode(NumericLineEdit::Int64Mode);
            ui->lineEditValue->setValue(params.Value.toLongLong());
            ui->controlBitPattern->setEnabled(false);
        break;

        case DataDisplayMode::UInt64:
        case DataDisplayMode::SwappedUInt64:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputMode(NumericLineEdit::UInt64Mode);
            ui->lineEditValue->setValue(params.Value.toULongLong());
            ui->controlBitPattern->setEnabled(false);
        break;
    }

    if(ui->controlBitPattern->isEnabled())
    {
        ui->controlBitPattern->setValue(_writeParams.Value.toUInt());

        connect(ui->lineEditValue, QOverload<const QVariant&>::of(&NumericLineEdit::valueChanged), this, [this](const QVariant& value) {
            ui->controlBitPattern->setValue(value.toUInt());
        });

        connect(ui->controlBitPattern, &BitPatternControl::valueChanged, this, [this](quint16 value) {
            ui->lineEditValue->setValue(value);
        });
    }
    else
    {
        delete ui->controlBitPattern;
        delete ui->groupBoxBitPattern;
        delete ui->labelBitPattern;
        adjustSize();
    }

    ui->lineEditValue->setFocus();
}

///
/// \brief DialogWriteHoldingRegister::~DialogWriteHoldingRegister
///
DialogWriteHoldingRegister::~DialogWriteHoldingRegister()
{
    delete ui;
}

///
/// \brief DialogWriteHoldingRegister::accept
///
void DialogWriteHoldingRegister::accept()
{
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Value = ui->lineEditValue->value<QVariant>();
    _writeParams.DeviceId = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}

///
/// \brief DialogWriteHoldingRegister::updateSimulationButton
///
void DialogWriteHoldingRegister::updateSimulationButton()
{
    switch(_simParams.Mode)
    {
        case SimulationMode::Disabled:
            ui->pushButtonSimulation->setEnabled(false);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: ON"));
            ui->pushButtonSimulation->setStyleSheet("padding: 4px 12px;");
        break;

        case SimulationMode::Off:
            ui->pushButtonSimulation->setEnabled(true);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: OFF"));
            ui->pushButtonSimulation->setStyleSheet("padding: 4px 12px;");
        break;

        default:
        {
            ui->pushButtonSimulation->setEnabled(true);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: ON"));

            const auto c = simColors(_simParams.DataMode);
            ui->pushButtonSimulation->setStyleSheet(QString(R"(
                    QPushButton {
                        color: white;
                        padding: 4px 12px;
                        background-color: %1;
                        border: 1px solid %2;
                        border-radius: 4px;
                    }
                    QPushButton:hover {
                        background-color: %3;
                    }
                    QPushButton:pressed {
                        background-color: %4;
                    }
                )").arg(c.base, c.border, c.hover, c.pressed));
        }
        break;
    }
}

///
/// \brief DialogWriteHoldingRegister::updateValue
///
void DialogWriteHoldingRegister::updateValue()
{
    ModbusClient* cli = _writeParams.Client;
    if(cli == nullptr || cli->state() != ModbusDevice::ConnectedState) return;

    WaitCursor wait(this);
    const quint8 deviceId = ui->lineEditNode->value<int>();
    const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
    const int count = registersCount(_writeParams.DisplayMode);
    const auto regs = cli->syncReadRegisters(QModbusDataUnit::HoldingRegisters, simAddr, count, deviceId);
    const auto value = makeValue(regs, _writeParams.DisplayMode, _writeParams.Order);
    if(value.isValid()) ui->lineEditValue->setValue(value);
}

///
/// \brief DialogWriteHoldingRegister::on_lineEditAddress_valueChanged
/// \param value
///
void DialogWriteHoldingRegister::on_lineEditAddress_valueChanged(const QVariant& value)
{
    if(_dataSimulator != nullptr)
    {
        const quint8 deviceId = ui->lineEditNode->value<int>();
        const int simAddr = value.toInt() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(deviceId, QModbusDataUnit::HoldingRegisters, simAddr);
        if(!_dataSimulator->canStartSimulation(_writeParams.DisplayMode, deviceId, QModbusDataUnit::HoldingRegisters, simAddr)) {
            _simParams.Mode = SimulationMode::Disabled;
        }
        updateSimulationButton();
    }
    updateValue();
}

///
/// \brief DialogWriteHoldingRegister::on_lineEditNode_valueChanged
/// \param value
///
void DialogWriteHoldingRegister::on_lineEditNode_valueChanged(const QVariant& value)
{
    if(_dataSimulator != nullptr)
    {
        const quint8 deviceId = value.toUInt();
        const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(deviceId, QModbusDataUnit::HoldingRegisters, simAddr);
        if(!_dataSimulator->canStartSimulation(_writeParams.DisplayMode, deviceId, QModbusDataUnit::HoldingRegisters, simAddr)) {
            _simParams.Mode = SimulationMode::Disabled;
        }
        updateSimulationButton();
    }
    updateValue();
}

///
/// \brief DialogWriteHoldingRegister::on_pushButtonSimulation_clicked
///
void DialogWriteHoldingRegister::on_pushButtonSimulation_clicked()
{
    if(_dataSimulator == nullptr) return;

    DialogAutoSimulation dlg(_writeParams.DisplayMode, _simParams, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        const quint8 deviceId = ui->lineEditNode->value<int>();
        const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        if(_simParams.Mode == SimulationMode::Off)
            _dataSimulator->stopSimulation(deviceId, QModbusDataUnit::HoldingRegisters, simAddr);
        else
            _dataSimulator->startSimulation(deviceId, QModbusDataUnit::HoldingRegisters, simAddr, _simParams);
        done(2);
    }
}
