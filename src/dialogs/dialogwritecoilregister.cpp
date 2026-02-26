#include "modbuslimits.h"
#include "modbusclient.h"
#include "waitcursor.h"
#include "datasimulator.h"
#include "dialogcoilsimulation.h"
#include "dialogwritecoilregister.h"
#include "ui_dialogwritecoilregister.h"

///
/// \brief DialogWriteCoilRegister::DialogWriteCoilRegister
/// \param params
/// \param hexAddress
/// \param dataSimulator
/// \param parent
///
DialogWriteCoilRegister::DialogWriteCoilRegister(ModbusWriteParams& params, bool hexAddress,
                                                 DataSimulator* dataSimulator, QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogWriteCoilRegister)
    ,_writeParams(params)
    ,_dataSimulator(dataSimulator)
{
    ui->setupUi(this);

    if(params.ForceModbus15And16Func) {
        setWindowTitle(tr("15: Write Single Coil"));
    }

    ui->lineEditNode->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditNode->setValue(params.DeviceId);

    ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditAddress->setValue(params.Address);

    ui->radioButtonOn->setChecked(params.Value.toBool());
    ui->radioButtonOff->setChecked(!params.Value.toBool());

    if(_dataSimulator != nullptr)
    {
        const int simAddr = params.Address - (params.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(params.DeviceId, QModbusDataUnit::Coils, simAddr);
    }
    updateSimulationButton();

    if(ui->radioButtonOff->isChecked())
        ui->radioButtonOn->setFocus();
    else
        ui->radioButtonOff->setFocus();
}

///
/// \brief DialogWriteCoilRegister::~DialogWriteCoilRegister
///
DialogWriteCoilRegister::~DialogWriteCoilRegister()
{
    delete ui;
}

///
/// \brief DialogWriteCoilRegister::accept
///
void DialogWriteCoilRegister::accept()
{
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Value = ui->radioButtonOn->isChecked();
    _writeParams.DeviceId = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}

///
/// \brief DialogWriteCoilRegister::updateSimulationButton
///
void DialogWriteCoilRegister::updateSimulationButton()
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
            ui->pushButtonSimulation->setEnabled(true);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: ON"));
            ui->pushButtonSimulation->setStyleSheet(R"(
                        QPushButton {
                            color: white;
                            padding: 4px 12px;
                            background-color: #4CAF50;
                            border: 1px solid #3e8e41;
                            border-radius: 4px;
                        }
                        QPushButton:hover {
                            background-color: #45a049;
                        }
                        QPushButton:pressed {
                            background-color: #3e8e41;
                        }
                    )");
        break;
    }
}

///
/// \brief DialogWriteCoilRegister::updateValue
///
void DialogWriteCoilRegister::updateValue()
{
    ModbusClient* cli = _writeParams.Client;
    if(cli == nullptr || cli->state() != ModbusDevice::ConnectedState) return;

    WaitCursor wait(this);
    const quint8 deviceId = ui->lineEditNode->value<int>();
    const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
    const bool coilValue = cli->syncReadRegister(QModbusDataUnit::Coils, simAddr, deviceId) != 0;
    ui->radioButtonOn->setChecked(coilValue);
    ui->radioButtonOff->setChecked(!coilValue);
}

///
/// \brief DialogWriteCoilRegister::on_lineEditAddress_valueChanged
/// \param value
///
void DialogWriteCoilRegister::on_lineEditAddress_valueChanged(const QVariant& value)
{
    if(_dataSimulator != nullptr)
    {
        const quint8 deviceId = ui->lineEditNode->value<int>();
        const int simAddr = value.toInt() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(deviceId, QModbusDataUnit::Coils, simAddr);
        updateSimulationButton();
    }
    updateValue();
}

///
/// \brief DialogWriteCoilRegister::on_lineEditNode_valueChanged
/// \param value
///
void DialogWriteCoilRegister::on_lineEditNode_valueChanged(const QVariant& value)
{
    if(_dataSimulator != nullptr)
    {
        const quint8 deviceId = value.toUInt();
        const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(deviceId, QModbusDataUnit::Coils, simAddr);
        updateSimulationButton();
    }
    updateValue();
}

///
/// \brief DialogWriteCoilRegister::on_pushButtonSimulation_clicked
///
void DialogWriteCoilRegister::on_pushButtonSimulation_clicked()
{
    if(_dataSimulator == nullptr) return;

    DialogCoilSimulation dlg(_simParams, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        const quint8 deviceId = ui->lineEditNode->value<int>();
        const int simAddr = ui->lineEditAddress->value<int>() - (_writeParams.ZeroBasedAddress ? 0 : 1);
        if(_simParams.Mode == SimulationMode::Off)
            _dataSimulator->stopSimulation(deviceId, QModbusDataUnit::Coils, simAddr);
        else
            _dataSimulator->startSimulation(deviceId, QModbusDataUnit::Coils, simAddr, _simParams);
        done(2);
    }
}
