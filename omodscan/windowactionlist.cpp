#include "windowactionlist.h"

///
/// \brief WindowActionList::WindowActionList
/// \param menu
///
WindowActionList::WindowActionList(QMenu* menu)
    : QObject{menu}
    ,_menu(menu)
{
    _actionWindows = new QAction("Windows...", _menu);
    connect(_actionWindows, &QAction::triggered, this, &WindowActionList::showWindowsDialog);

    _actionWindows->setVisible(false);
    _menu->addAction(_actionWindows);
}

///
/// \brief WindowActionList::isEmpty
/// \return
///
bool WindowActionList::isEmpty() const
{
    return _actionList.isEmpty();
}

///
/// \brief WindowActionList::addWindow
/// \param wnd
///
void WindowActionList::addWindow(QMdiSubWindow* wnd)
{
    if(!wnd) return;

    for(auto&& a : _actionList)
    {
        if(wnd == a->data().value<QMdiSubWindow*>()) return;
    }

    auto activateAction = new QAction(_menu);
    activateAction->setData(QVariant::fromValue(wnd));
    activateAction->setCheckable(true);

    connect(activateAction, &QAction::triggered, this, [this, wnd](bool)
    {
        emit triggered(wnd);
    });

    _actionList.append(activateAction);
    _menu->insertAction(_actionWindows, activateAction);

    updateMenu();
}

///
/// \brief WindowActionList::removeWindow
/// \param wnd
///
void WindowActionList::removeWindow(QMdiSubWindow* wnd)
{
    if(!wnd) return;

    QAction* activateAction = nullptr;
    for(auto&& a : _actionList)
    {
        if(wnd == a->data().value<QMdiSubWindow*>())
        {
            activateAction = a;
            break;
        }
    }

    if(!activateAction) return;

    _actionList.removeOne(activateAction);
    _menu->removeAction(activateAction);

    updateMenu();
}

///
/// \brief WindowActionList::update
///
void WindowActionList::update()
{
    updateMenu();
}

///
/// \brief WindowActionList::showWindowsDialog
///
void WindowActionList::showWindowsDialog()
{

}

///
/// \brief WindowActionList::updateMenu
///
void WindowActionList::updateMenu()
{
    int i = 0, ci = 0;
    const bool largeList = _actionList.size() > 9;
    for(auto&& a: _actionList)
    {
        const auto wnd = a->data().value<QMdiSubWindow*>();
        if(!wnd) continue;

        a->setChecked(wnd->property("isActive").toBool());
        a->setVisible(++i < 10 || a->isChecked());

        if(a->isChecked()) ci = i;
        if(largeList) _actionList[8]->setVisible(ci < 10);

        const int num = (i < 10) ? i : (a->isChecked() ? 9 : i);
        a->setText(QString("%1 %2").arg(QString::number(num), wnd->windowTitle()));
    }

    _actionWindows->setVisible(largeList);
}
