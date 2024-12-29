#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include "modbusclient.h"
#include "formmodsca.h"
#include "windowactionlist.h"
#include "recentfileactionlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setLanguage(const QString& lang);

signals:
    void modbusClientChanged(QModbusClient* cli);

protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private slots:
    void on_awake();

    /* File menu slots */
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionPrint_triggered();
    void on_actionPrintSetup_triggered();
    void on_actionExit_triggered();

    /* Connection menu slots */
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_actionQuickConnect_triggered();
    void on_actionEnable_triggered();
    void on_actionDisable_triggered();
    void on_actionSaveConfig_triggered();
    void on_actionRestoreNow_triggered();
    void on_actionModbusScanner_triggered();

    /* Setup menu slots*/
    void on_actionDataDefinition_triggered();
    void on_actionShowData_triggered();
    void on_actionShowTraffic_triggered();
    void on_actionBinary_triggered();
    void on_actionUInt16_triggered();
    void on_actionInt16_triggered();
    void on_actionInt32_triggered();
    void on_actionSwappedInt32_triggered();
    void on_actionUInt32_triggered();
    void on_actionSwappedUInt32_triggered();
    void on_actionInt64_triggered();
    void on_actionSwappedInt64_triggered();
    void on_actionUInt64_triggered();
    void on_actionSwappedUInt64_triggered();
    void on_actionHex_triggered();
    void on_actionAscii_triggered();
    void on_actionFloatingPt_triggered();
    void on_actionSwappedFP_triggered();
    void on_actionDblFloat_triggered();
    void on_actionSwappedDbl_triggered();
    void on_actionLittleEndian_triggered();
    void on_actionBigEndian_triggered();
    void on_actionHexAddresses_triggered();
    void on_actionForceCoils_triggered();
    void on_actionPresetRegs_triggered();
    void on_actionMaskWrite_triggered();
    void on_actionUserMsg_triggered();
    void on_actionMsgParser_triggered();
    void on_actionAddressScan_triggered();
    void on_actionTextCapture_triggered();
    void on_actionCaptureOff_triggered();
    void on_actionResetCtrs_triggered();

    /* View menu slots */
    void on_actionToolbar_triggered();
    void on_actionStatusBar_triggered();
    void on_actionDisplayBar_triggered();
    void on_actionBackground_triggered();
    void on_actionForeground_triggered();
    void on_actionStatus_triggered();
    void on_actionFont_triggered();

    /* Language menu slots */
    void on_actionEnglish_triggered();
    void on_actionRussian_triggered();
    void on_actionChinese_triggered();

    /* Window menu slots */
    void on_actionCascade_triggered();
    void on_actionTile_triggered();
    void on_actionWindows_triggered();

    /* Help menu slots */
    void on_actionAbout_triggered();

    void on_modbusError(const QString& error, int requestId);
    void on_modbusConnectionError(const QString& error);
    void on_modbusConnected(const ConnectionDetails& cd);
    void on_modbusDisconnected(const ConnectionDetails& cd);

    void updateMenuWindow();
    void openFile(const QString& filename);
    void windowActivate(QMdiSubWindow* wnd);

private:
    void addRecentFile(const QString& filename);
    void updateDataDisplayMode(DataDisplayMode mode);

    FormModSca* createMdiChild(int id);
    FormModSca* currentMdiChild() const;
    FormModSca* findMdiChild(int id) const;
    FormModSca* firstMdiChild() const;

    FormModSca* loadMdiChild(const QString& filename);
    void saveMdiChild(FormModSca* frm);

    void loadConfig(const QString& filename);
    void saveConfig(const QString& filename);

    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;

    QString _lang;
    QTranslator _qtTranslator;
    QTranslator _appTranslator;

    QIcon _icoBigEndian;
    QIcon _icoLittleEndian;

private:
    int _windowCounter;
    bool _autoStart;
    QString _fileAutoStart;
    ConnectionDetails _connParams;
    ModbusClient _modbusClient;

    WindowActionList* _windowActionList;
    RecentFileActionList* _recentFileActionList;
    QPrinter* _selectedPrinter;
    DataSimulator* _dataSimulator;
};
#endif // MAINWINDOW_H
