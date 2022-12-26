#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
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
    void on_awake();

private:
    void updateMenus();
    void updateDataDisplayMode(DataDisplayMode mode);

private:
    FormModSca* createMdiChild();
    FormModSca* currentMdiChild();

private:
    Ui::MainWindow *ui;

private:
    AppSettings _settings;
};
#endif // MAINWINDOW_H
