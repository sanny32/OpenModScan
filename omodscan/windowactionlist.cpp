#include "windowactionlist.h"

///
/// \brief WindowActionList::WindowActionList
/// \param menu
///
WindowActionList::WindowActionList(QMenu* menu)
    : QObject{menu}
    ,_menu(menu)
{
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
    _menu->addAction(activateAction);

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
/// \brief WindowActionList::updateMenu
///
void WindowActionList::updateMenu()
{
    int i = 0;
    for(auto&& a : _actionList)
    {
        auto wnd = a->data().value<QMdiSubWindow*>();
        if(!wnd) continue;

        a->setChecked(wnd->property("isActive").toBool());
        a->setText(QString("%1 %2").arg(QString::number(++i), wnd->windowTitle()));
    }
}
