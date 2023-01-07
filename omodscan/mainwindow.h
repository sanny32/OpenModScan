#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusclient.h"
#include "appsettings.h"
#include "formmodsca.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void modbusClientChanged(QModbusClient* cli);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_actionQuickConnect_triggered();
    void on_actionDataDefinition_triggered();
    void on_actionShowData_triggered();
    void on_actionShowTraffic_triggered();
    void on_actionBinary_triggered();
    void on_actionUnsignedDecimal_triggered();
    void on_actionInteger_triggered();
    void on_actionHex_triggered();
    void on_actionFloatingPt_triggered();
    void on_actionSwappedFP_triggered();
    void on_actionDblFloat_triggered();
    void on_actionSwappedDbl_triggered();
    void on_actionHexAddresses_triggered();
    void on_actionForceCoils_triggered();
    void on_actionPresetRegs_triggered();
    void on_actionMaskWrite_triggered();
    void on_actionUserMsg_triggered();
    void on_actionTextCapture_triggered();
    void on_actionCaptureOff_triggered();
    void on_actionResetCtrs_triggered();
    void on_actionToolbar_triggered();
    void on_actionStatusBar_triggered();
    void on_actionDsiplayBar_triggered();
    void on_awake();

    void on_modbusError(const QString& error);
    void on_modbusConnectionError(const QString& error);

private:
    void updateMenus();
    void updateDataDisplayMode(DataDisplayMode mode);

    FormModSca* createMdiChild();
    FormModSca* currentMdiChild();

private:
    Ui::MainWindow *ui;

private:
    AppSettings _settings;
    ModbusClient _modbusClient;
};
#endif // MAINWINDOW_H