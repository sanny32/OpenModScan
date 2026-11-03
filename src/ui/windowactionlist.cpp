#include "windowactionlist.h"

///
/// \brief WindowActionList::WindowActionList
/// \param menu
///
WindowActionList::WindowActionList(QMenu* menu, QAction* placeHolder)
    : QObject{menu}
    ,_menu(menu)
    ,_placeHolder(placeHolder)
{
    _placeHolder->setVisible(false);
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
/// \brief WindowActionList::actionList
/// \return
///
QList<QAction*> WindowActionList::actionList() const
{
    return _actionList;
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
    _menu->insertAction(_placeHolder, activateAction);

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

    _placeHolder->setVisible(largeList);
}
