#include <QMenu>
#include <QSignalBlocker>
#include <QtGlobal>
#include "modbuslimits.h"
#include "modbusclient.h"
#include "waitcursor.h"
#include "datasimulator.h"
#include "coloredpushbutton.h"
#include "coloredtoolbutton.h"
#include "dialogpulsemode.h"
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
DialogWriteCoilRegister::DialogWriteCoilRegister(ModbusWriteParams& params, const DisplayDefinition& dd,
                                                 DataSimulator* dataSimulator, QWidget *parent)
    : QFixedSizeDialog(parent),
      ui(new Ui::DialogWriteCoilRegister)
    ,_writeParams(params)
    ,_dataSimulator(dataSimulator)
{
    ui->setupUi(this);

    if(params.ForceModbus15And16Func) {
        setWindowTitle(tr("15: Write Single Coil"));
    }

    {
        QSignalBlocker nodeBlocker(ui->lineEditNode);
        QSignalBlocker addressBlocker(ui->lineEditAddress);

        ui->lineEditNode->setLeadingZeroes(params.LeadingZeros);
        ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
        ui->lineEditNode->setValue(params.DeviceId);
        ui->lineEditNode->setHexButtonVisible(true);
        ui->lineEditNode->setHexView(dd.HexViewDeviceId);

        ui->lineEditAddress->setLeadingZeroes(params.LeadingZeros);
        ui->lineEditAddress->setInputMode(dd.HexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
        ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
        ui->lineEditAddress->setValue(params.Address);
        ui->lineEditAddress->setHexButtonVisible(true);
        ui->lineEditAddress->setHexView(dd.HexViewAddress);
    }

    ui->radioButtonOn->setChecked(params.Value.toBool());
    ui->radioButtonOff->setChecked(!params.Value.toBool());

    if(_dataSimulator != nullptr)
    {
        const int simAddr = params.Address - (params.ZeroBasedAddress ? 0 : 1);
        _simParams = _dataSimulator->simulationParams(params.DeviceId, QModbusDataUnit::Coils, simAddr);
    }
    updateSimulationButton();

    setupPulseButton();
    updatePulseButton();

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
            ui->pushButtonSimulation->clearColors();
        break;

        case SimulationMode::Off:
            ui->pushButtonSimulation->setEnabled(true);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: OFF"));
            ui->pushButtonSimulation->clearColors();
        break;

        default:
            ui->pushButtonSimulation->setEnabled(true);
            ui->pushButtonSimulation->setText(tr("Auto Simulation: ON"));
            ui->pushButtonSimulation->setColors({ "#4CAF50", "#45A049", "#3E8E41", "#3E8E41" });
        break;
    }
}

///
/// \brief DialogWriteCoilRegister::setupPulseButton
///
void DialogWriteCoilRegister::setupPulseButton()
{
    connect(ui->actionPulseMode, &QAction::triggered, this, [this](){
        DialogPulseMode dlg(_writeParams.PusleParams, this);
        if(dlg.exec() == QDialog::Accepted)
            updatePulseButton();
    });

    auto menu = new QMenu();
    menu->addAction(ui->actionPulseMode);
    ui->toolButtonPulse->setMenu(menu);

    const auto currentText = ui->toolButtonPulse->text();
    const auto currentStyleSheet = ui->toolButtonPulse->styleSheet();

    ui->toolButtonPulse->setText(tr("Pulse: OFF"));
    ui->toolButtonPulse->clearColors();
    const int offWidth = ui->toolButtonPulse->sizeHint().width();

    ui->toolButtonPulse->setText(tr("Pulse: ON"));
    ui->toolButtonPulse->setColors({ "#F0A43A", "#E2952E", "#D58422", "#B96E16" });
    const int onWidth = ui->toolButtonPulse->sizeHint().width();

    ui->toolButtonPulse->setMinimumWidth(qMax(offWidth, onWidth));
    ui->toolButtonPulse->setText(currentText);
    ui->toolButtonPulse->setStyleSheet(currentStyleSheet);
}

///
/// \brief DialogWriteCoilRegister::updatePulseButton
///
void DialogWriteCoilRegister::updatePulseButton()
{
    if( _writeParams.PusleParams.Enabled) {
        ui->toolButtonPulse->setText(tr("Pulse: ON"));
        ui->toolButtonPulse->setColors({ "#F0A43A", "#E2952E", "#D58422", "#B96E16" });
    }
    else {
        ui->toolButtonPulse->setText(tr("Pulse: OFF"));
        ui->toolButtonPulse->clearColors();
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
/// \brief DialogWriteCoilRegister::on_toolButtonPulse_clicked
///
void DialogWriteCoilRegister::on_toolButtonPulse_clicked()
{
    _writeParams.PusleParams.Enabled = !_writeParams.PusleParams.Enabled;
    updatePulseButton();
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
