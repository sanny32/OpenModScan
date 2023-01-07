#include <QtWidgets>
#include "dialogdisplaydefinition.h"
#include "dialogconnectiondetails.h"
#include "dialogmaskwriteregiter.h"
#include "dialogsetuppresetdata.h"
#include "dialogforcemultiplecoils.h"
#include "dialogforcemultipleregisters.h"
#include "dialogusermsg.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

///
/// \brief MainWindow::MainWindow
/// \param parent
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_modbusClient(nullptr)
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    auto dispatcher = QAbstractEventDispatcher::instance();
    connect(dispatcher, &QAbstractEventDispatcher::awake, this, &MainWindow::on_awake);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);
    connect(&_modbusClient, &ModbusClient::modbusError, this, &MainWindow::on_modbusError);
    connect(&_modbusClient, &ModbusClient::modbusConnectionError, this, &MainWindow::on_modbusConnectionError);

    ui->actionNew->trigger();
}

///
/// \brief MainWindow::~MainWindow
///
MainWindow::~MainWindow()
{
    delete ui;
}

///
/// \brief MainWindow::closeEvent
/// \param event
///
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::on_awake()
{
    auto frm = currentMdiChild();
    const auto state = _modbusClient.state();

    ui->menuSetup->setEnabled(frm != nullptr);
    ui->menuWindow->setEnabled(frm != nullptr);

    ui->actionSave->setEnabled(frm != nullptr);
    ui->actionSaveAs->setEnabled(frm != nullptr);
    ui->actionConnect->setEnabled(state == QModbusDevice::UnconnectedState);
    ui->actionDisconnect->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionQuickConnect->setEnabled(state == QModbusDevice::UnconnectedState);
    ui->actionDataDefinition->setEnabled(frm != nullptr);
    ui->actionShowData->setEnabled(frm != nullptr);
    ui->actionShowTraffic->setEnabled(frm != nullptr);
    ui->actionBinary->setEnabled(frm != nullptr);
    ui->actionUnsignedDecimal->setEnabled(frm != nullptr);
    ui->actionInteger->setEnabled(frm != nullptr);
    ui->actionHex->setEnabled(frm != nullptr);
    ui->actionFloatingPt->setEnabled(frm != nullptr);
    ui->actionSwappedFP->setEnabled(frm != nullptr);
    ui->actionDblFloat->setEnabled(frm != nullptr);
    ui->actionSwappedDbl->setEnabled(frm != nullptr);
    ui->actionForceCoils->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionPresetRegs->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionMaskWrite->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionUserMsg->setEnabled(state == QModbusDevice::ConnectedState);
    ui->actionTextCapture->setEnabled(frm != nullptr);
    ui->actionCaptureOff->setEnabled(frm != nullptr);
    ui->actionResetCtrs->setEnabled(frm != nullptr);
    ui->actionToolbar->setChecked(ui->toolBarMain->isVisible());
    ui->actionStatusBar->setChecked(ui->statusbar->isVisible());
    ui->actionDsiplayBar->setChecked(ui->toolBarDisplay->isVisible());
    ui->actionPrint->setEnabled(frm != nullptr);

    if(frm != nullptr)
    {
        const auto ddm = frm->dataDisplayMode();
        ui->actionBinary->setChecked(ddm == DataDisplayMode::Binary);
        ui->actionUnsignedDecimal->setChecked(ddm == DataDisplayMode::Decimal);
        ui->actionInteger->setChecked(ddm == DataDisplayMode::Integer);
        ui->actionHex->setChecked(ddm == DataDisplayMode::Hex);
        ui->actionFloatingPt->setChecked(ddm == DataDisplayMode::FloatingPt);
        ui->actionSwappedFP->setChecked(ddm == DataDisplayMode::SwappedFP);
        ui->actionDblFloat->setChecked(ddm == DataDisplayMode::DblFloat);
        ui->actionSwappedDbl->setChecked(ddm == DataDisplayMode::SwappedDbl);

        ui->actionHexAddresses->setChecked(frm->displayHexAddreses());

        const auto dm = frm->displayMode();
        ui->actionShowData->setChecked(dm == DisplayMode::Data);
        ui->actionShowTraffic->setChecked(dm == DisplayMode::Traffic);

        ui->actionTextCapture->setEnabled(!frm->hasTextCapture());
        ui->actionCaptureOff->setEnabled(frm->hasTextCapture());
    }
}

///
/// \brief MainWindow::on_modbusWriteError
/// \param error
///
void MainWindow::on_modbusError(const QString& error)
{
    QMessageBox::warning(this, windowTitle(), error);
}

///
/// \brief MainWindow::on_modbusConnectionError
/// \param error
///
void MainWindow::on_modbusConnectionError(const QString& error)
{
    QMessageBox::warning(this, windowTitle(), error);
}

///
/// \brief MainWindow::on_actionNew_triggered
///
void MainWindow::on_actionNew_triggered()
{
    auto frm = createMdiChild();
    //frm->setDataDisplayMode(_settings.DataDisplayMode);
    frm->show();
}

///
/// \brief MainWindow::on_actionConnect_triggered
///
void MainWindow::on_actionConnect_triggered()
{
    DialogConnectionDetails dlg(_settings.ConnectionParams, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        ui->actionQuickConnect->trigger();
    }
}

///
/// \brief MainWindow::on_actionDisconnect_triggered
///
void MainWindow::on_actionDisconnect_triggered()
{
    _modbusClient.disconnectDevice();
}

///
/// \brief MainWindow::on_actionQuickConnect_triggered
///
void MainWindow::on_actionQuickConnect_triggered()
{
    _modbusClient.connectDevice(_settings.ConnectionParams);
}

///
/// \brief MainWindow::on_actionDataDefinition_triggered
///
void MainWindow::on_actionDataDefinition_triggered()
{
     auto frm = currentMdiChild();
     if(frm)
     {
        DialogDisplayDefinition dlg(frm);
        dlg.exec();
     }
}

///
/// \brief MainWindow::on_actionShowData_triggered
///
void MainWindow::on_actionShowData_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->setDisplayMode(DisplayMode::Data);
}

///
/// \brief MainWindow::on_actionShowTraffic_triggered
///
void MainWindow::on_actionShowTraffic_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->setDisplayMode(DisplayMode::Traffic);
}

///
/// \brief MainWindow::on_actionBinary_triggered
///
void MainWindow::on_actionBinary_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Binary);
}

///
/// \brief MainWindow::on_actionUnsignedDecimal_triggered
///
void MainWindow::on_actionUnsignedDecimal_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Decimal);
}

///
/// \brief MainWindow::on_actionInteger_triggered
///
void MainWindow::on_actionInteger_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Integer);
}

///
/// \brief MainWindow::on_actionHex_triggered
///
void MainWindow::on_actionHex_triggered()
{
    updateDataDisplayMode(DataDisplayMode::Hex);
}

///
/// \brief MainWindow::on_actionFloatingPt_triggered
///
void MainWindow::on_actionFloatingPt_triggered()
{
    updateDataDisplayMode(DataDisplayMode::FloatingPt);
}

///
/// \brief MainWindow::on_actionSwappedFP_triggered
///
void MainWindow::on_actionSwappedFP_triggered()
{
    updateDataDisplayMode(DataDisplayMode::SwappedFP);
}

///
/// \brief MainWindow::on_actionDblFloat_triggered
///
void MainWindow::on_actionDblFloat_triggered()
{
    updateDataDisplayMode(DataDisplayMode::DblFloat);
}

///
/// \brief MainWindow::on_actionSwappedDbl_triggered
///
void MainWindow::on_actionSwappedDbl_triggered()
{
    updateDataDisplayMode(DataDisplayMode::SwappedDbl);
}

///
/// \brief MainWindow::on_actionHexAddresses_triggered
///
void MainWindow::on_actionHexAddresses_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->setDisplayHexAddreses(!frm->displayHexAddreses());
}

///
/// \brief MainWindow::on_actionForceCoils_triggered
///
void MainWindow::on_actionForceCoils_triggered()
{
    auto frm = currentMdiChild();
    if(!frm) return;

    const auto dd = frm->displayDefinition();
    SetupPresetParams presetParams = { dd.DeviceId, dd.PointAddress, dd.Length };

    {
        DialogSetupPresetData dlg(presetParams, QModbusDataUnit::Coils, this);
        if(dlg.exec() != QDialog::Accepted) return;
    }

    ModbusWriteParams params;
    params.Node = presetParams.SlaveAddress;
    params.Address = presetParams.PointAddress;

    if(dd.PointType == QModbusDataUnit::Coils)
    {
        params.Value = QVariant::fromValue(frm->data());
    }

    DialogForceMultipleCoils dlg(params, presetParams.Length, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        _modbusClient.writeRegister(QModbusDataUnit::Coils, params, 0);
    }
}

///
/// \brief MainWindow::on_actionPresetRegs_triggered
///
void MainWindow::on_actionPresetRegs_triggered()
{
    auto frm = currentMdiChild();
    if(!frm) return;

    const auto dd = frm->displayDefinition();
    SetupPresetParams presetParams = { dd.DeviceId, dd.PointAddress, dd.Length };

    {
        DialogSetupPresetData dlg(presetParams, QModbusDataUnit::HoldingRegisters, this);
        if(dlg.exec() != QDialog::Accepted) return;
    }

    ModbusWriteParams params;
    params.Node = presetParams.SlaveAddress;
    params.Address = presetParams.PointAddress;
    params.DisplayMode = frm->dataDisplayMode();

    if(dd.PointType == QModbusDataUnit::HoldingRegisters)
    {
        params.Value = QVariant::fromValue(frm->data());
    }

    DialogForceMultipleRegisters dlg(params, presetParams.Length, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        _modbusClient.writeRegister(QModbusDataUnit::HoldingRegisters, params, 0);
    }
}

///
/// \brief MainWindow::on_actionMaskWrite_triggered
///
void MainWindow::on_actionMaskWrite_triggered()
{
    ModbusMaskWriteParams params = { 1, 1, 0xFFFF, 0};
    DialogMaskWriteRegiter dlg(params, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        _modbusClient.maskWriteRegister(params, 0);
    }
}

///
/// \brief MainWindow::on_actionUserMsg_triggered
///
void MainWindow::on_actionUserMsg_triggered()
{
    auto frm = currentMdiChild();
    if(!frm) return;

    const auto dd = frm->displayDefinition();
    const auto mode = frm->dataDisplayMode();

    DialogUserMsg dlg(dd.DeviceId, mode, _modbusClient, this);
    dlg.exec();
}

///
/// \brief MainWindow::on_actionTextCapture_triggered
///
void MainWindow::on_actionTextCapture_triggered()
{
    auto frm = currentMdiChild();
    if(!frm) return;

    const auto filename = QFileDialog::getSaveFileName(this, "Saving...", QString(), "Text files (*.txt)");
    frm->startTextCapture(filename);
}

///
/// \brief MainWindow::on_actionCaptureOff_triggered
///
void MainWindow::on_actionCaptureOff_triggered()
{
    auto frm = currentMdiChild();
    if(!frm) return;

    frm->stopTextCapture();
}

///
/// \brief MainWindow::on_actionResetCtrs_triggered
///
void MainWindow::on_actionResetCtrs_triggered()
{
    auto frm = currentMdiChild();
    if(frm) frm->resetCtrs();
}

///
/// \brief MainWindow::on_actionToolbar_triggered
///
void MainWindow::on_actionToolbar_triggered()
{
    ui->toolBarMain->setVisible(!ui->toolBarMain->isVisible());
}

///
/// \brief MainWindow::on_actionStatusBar_triggered
///
void MainWindow::on_actionStatusBar_triggered()
{
    ui->statusbar->setVisible(!ui->statusbar->isVisible());
}

///
/// \brief MainWindow::on_actionDsiplayBar_triggered
///
void MainWindow::on_actionDsiplayBar_triggered()
{
    ui->toolBarDisplay->setVisible(!ui->toolBarDisplay->isVisible());
}

///
/// \brief MainWindow::updateMenus
///
void MainWindow::updateMenus()
{
}

///
/// \brief MainWindow::updateDisplayMode
/// \param mode
///
void MainWindow::updateDataDisplayMode(DataDisplayMode mode)
{
    //_settings.DataDisplayMode = mode;
    auto frm = currentMdiChild();
    if(frm) frm->setDataDisplayMode(mode);
}

///
/// \brief MainWindow::createMdiChild
/// \return
///
FormModSca* MainWindow::createMdiChild()
{
    const auto num = ui->mdiArea->subWindowList().count() + 1;
    auto frm = new FormModSca(num, _modbusClient, this);
    ui->mdiArea->addSubWindow(frm);
    return frm;
}

///
/// \brief MainWindow::currentMdiChild
/// \return
///
FormModSca* MainWindow::currentMdiChild()
{
    auto child = ui->mdiArea->currentSubWindow();
    return child ? (FormModSca*)child->widget() : nullptr;
}
