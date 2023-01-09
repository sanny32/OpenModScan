#include <QFileInfo>
#include "recentfileactionlist.h"

///
/// \brief RecentFileActionList::RecentFileActionList
/// \param menu
/// \param holder
/// \param parent
///
RecentFileActionList::RecentFileActionList(QMenu* menu, QAction* holder, QObject* parent)
    : QObject(parent)
    ,_menu(menu)
    ,_holder(holder)
    ,_insert(nullptr)
{
    Q_ASSERT(_menu != nullptr);
    Q_ASSERT(_holder != nullptr);

    _holder->setVisible(true);

    const auto actions = _menu->actions();
    for(int i = 0; i < actions.count() ; i++)
    {
        if(actions.at(i) == _holder && i + 1 < actions.count())
        {
            _insert = actions.at(i + 1);
            break;
        }
    }
}

///
/// \brief RecentFileActionList::isEmpty
/// \return
///
bool RecentFileActionList::isEmpty() const
{
    return _actionList.isEmpty();
}

///
/// \brief RecentFileActionList::addRecentFile
/// \param filename
///
void RecentFileActionList::addRecentFile(const QString& filename)
{
    _holder->setVisible(false);

    for(auto&& a : _actionList)
    {
        if(a->data().toString() == filename) return;
    }

    const auto num = _actionList.count() + 1;
    const auto name = QString("%1 %2").arg(QString::number(num), QFileInfo(filename).fileName());
    auto openAction = new QAction(name, this);
    openAction->setData(filename);

    connect(openAction, &QAction::triggered, this, [this, filename]
    {
        emit triggered(filename);
    });

    _actionList.append(openAction);

    _menu->insertAction(_insert, openAction);
}

///
/// \brief RecentFileActionList::removeRecentFile
/// \param filename
///
void RecentFileActionList::removeRecentFile(const QString& filename)
{
    QAction* openAction = nullptr;
    for(auto&& a : _actionList)
    {
        if(a->data().toString() == filename)
            openAction = a;
    }

    if(!openAction) return;

    _actionList.removeOne(openAction);
    _menu->removeAction(openAction);

    updateMenu();
}

///
/// \brief RecentFileActionList::updateMenu
///
void RecentFileActionList::updateMenu()
{
    if(isEmpty())
    {
        _holder->setVisible(true);
    }
    else
    {
        int i = 0;
        for(auto&& a : _actionList)
        {
            const auto filename = a->data().toString();
            a->setText(QString("%1 %2").arg(QString::number(++i), QFileInfo(filename).fileName()));
        }
    }
}
