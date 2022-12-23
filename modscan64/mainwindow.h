#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void updateMenus();

private:
    FormModSca* createMdiChild();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
