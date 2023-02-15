#include <QFileInfo>
#include <QSettings>
#include "recentfileactionlist.h"

///
/// \brief RecentFileActionList::RecentFileActionList
/// \param menu
/// \param holder
///
RecentFileActionList::RecentFileActionList(QMenu* menu, QAction* placeholder)
    : QObject(menu)
    ,_menu(menu)
    ,_placeholder(placeholder)
    ,_placeinserter(placeholder)
{
    Q_ASSERT(_menu != nullptr);
    Q_ASSERT(_placeholder != nullptr);

    _placeholder->setVisible(true);

    const auto actions = _menu->actions();
    for(int i = 0; i < actions.count() ; i++)
    {
        if(actions.at(i) == _placeholder && i + 1 < actions.count())
        {
            _placeinserter = actions.at(i + 1);
            break;
        }
    }

    QSettings m(QSettings::NativeFormat, QSettings::UserScope, APP_NAME, APP_VERSION);
    const int size = m.beginReadArray("RecentFiles");
    for (int i = 0; i < size; ++i)
    {
        m.setArrayIndex(i);
        addRecentFile(m.value("Name").toString());
    }
    m.endArray();
}

///
/// \brief RecentFileActionList::~RecentFileActionList
///
RecentFileActionList::~RecentFileActionList()
{
    QSettings m(QSettings::NativeFormat, QSettings::UserScope, APP_NAME, APP_VERSION);
    m.beginWriteArray("RecentFiles", _actionList.size());
    for(int i = 0; i < _actionList.size(); i++)
    {
        m.setArrayIndex(i);
        m.setValue("Name", _actionList[i]->data().toString());
    }
    m.endArray();
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
    if(filename.isEmpty()) return;
    removeRecentFile(filename);

    const auto num = _actionList.count() + 1;
    const auto name = QString("%1 %2").arg(QString::number(num), QFileInfo(filename).fileName());
    auto openAction = new QAction(name, _menu);
    openAction->setData(filename);

    connect(openAction, &QAction::triggered, this, [this, filename]
    {
        emit triggered(filename);
    });

    _actionList.append(openAction);
    if(_actionList.size() > 10)
    {
        _menu->removeAction(_actionList.first());
        _actionList.removeFirst();
    }

    _menu->insertAction(_placeinserter, openAction);
    updateMenu();
}

///
/// \brief RecentFileActionList::removeRecentFile
/// \param filename
///
void RecentFileActionList::removeRecentFile(const QString& filename)
{
    if(filename.isEmpty()) return;

    QAction* openAction = nullptr;
    for(auto&& a : _actionList)
    {
        if(a->data().toString() == filename)
        {
            openAction = a;
            break;
        }
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
    if(!isEmpty())
    {
        int i = 0;
        for(auto&& a : _actionList)
        {
            const auto filename = a->data().toString();
            a->setText(QString("%1 %2").arg(QString::number(++i), QFileInfo(filename).fileName()));
        }
    }

    _placeholder->setVisible(isEmpty());
}
