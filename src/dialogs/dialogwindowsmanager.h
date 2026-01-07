#ifndef DIALOGWINDOWSMANAGER_H
#define DIALOGWINDOWSMANAGER_H

#include <QDialog>
#include <QMdiSubWindow>
#include <QListWidgetItem>

namespace Ui {
class DialogWindowsManager;
}

///
/// \brief The DialogWindowsManager class
///
class DialogWindowsManager : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWindowsManager(const QList<QAction*>& actions, QAction* saveAction, QWidget *parent = nullptr);
    ~DialogWindowsManager();

private slots:
    void on_pushButtonActivate_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonClose_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void activateWindow(QListWidgetItem *item);

private:
    Ui::DialogWindowsManager *ui;

private:
    QAction* _saveAction;
    QList<QAction*> _windowActions;
};

#endif // DIALOGWINDOWSMANAGER_H
