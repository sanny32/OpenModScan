#include <QAction>
#include <QAbstractEventDispatcher>
#include "dialogwindowsmanager.h"
#include "ui_dialogwindowsmanager.h"

///
/// \brief DialogWindowsManager::DialogWindowsManager
/// \param actions
/// \param saveAction
/// \param parent
///
DialogWindowsManager::DialogWindowsManager(const QList<QAction*>& actions, QAction* saveAction, QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::DialogWindowsManager)
    ,_saveAction(saveAction)
    ,_windowActions(actions)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    for(auto&& a : _windowActions)
    {
        const auto wnd = a->data().value<QMdiSubWindow*>();
        if(!wnd) continue;

        auto item = new QListWidgetItem(wnd->windowTitle(), ui->listWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(a));
        ui->listWidget->addItem(item);

        if(wnd->property("isActive").toBool())
            ui->listWidget->setCurrentItem(item);

    }

    connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, this, [this]
    {
        const auto item = ui->listWidget->currentItem();
        ui->pushButtonActivate->setEnabled(item != nullptr);
        ui->pushButtonClose->setEnabled(item != nullptr);
        ui->pushButtonSave->setEnabled(item != nullptr && _saveAction != nullptr);
    });
}

///
/// \brief DialogWindowsManager::~DialogWindowsManager
///
DialogWindowsManager::~DialogWindowsManager()
{
    delete ui;
}

///
/// \brief DialogWindowsManager::activateWindow
/// \param item
///
void DialogWindowsManager::activateWindow(QListWidgetItem *item)
{
    if(!item) return;

    const auto action = item->data(Qt::UserRole).value<QAction*>();
    if(action) action->trigger();
}

///
/// \brief DialogWindowsManager::on_listWidget_itemDoubleClicked
///
void DialogWindowsManager::on_listWidget_itemDoubleClicked(QListWidgetItem*)
{
    ui->pushButtonActivate->click();
}

///
/// \brief DialogWindowsManager::on_pushButtonActivate_clicked
///
void DialogWindowsManager::on_pushButtonActivate_clicked()
{
    activateWindow(ui->listWidget->currentItem());
    close();
}

///
/// \brief DialogWindowsManager::on_pushButtonSave_clicked
///
void DialogWindowsManager::on_pushButtonSave_clicked()
{
    if(_saveAction)
    {
        activateWindow(ui->listWidget->currentItem());
        _saveAction->trigger();
    }
}

///
/// \brief DialogWindowsManager::on_pushButtonClose_clicked
///
void DialogWindowsManager::on_pushButtonClose_clicked()
{
    auto item = ui->listWidget->currentItem();
    if(!item) return;

    const auto action = item->data(Qt::UserRole).value<QAction*>();
    if(!action) return;

    auto wnd = action->data().value<QMdiSubWindow*>();
    if(!wnd) return;

    wnd->close();
    delete item;
}
