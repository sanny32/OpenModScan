#ifndef RECENTFILEACTIONLIST_H
#define RECENTFILEACTIONLIST_H

#include <QMenu>

///
/// \brief The RecentFileActionList class
///
class RecentFileActionList : public QObject
{
    Q_OBJECT

public:
    explicit RecentFileActionList(QMenu* menu, QAction* placeholder);
    ~RecentFileActionList() override;

    bool isEmpty() const;

    void addRecentFile(const QString& filename);
    void removeRecentFile(const QString& filename);

signals:
    void triggered(const QString& filename);

private:
    void updateMenu();

private:
    QMenu* _menu;
    QAction* _placeholder;
    QAction* _placeinserter;
    QList<QAction*> _actionList;
};

#endif // RECENTFILEACTIONLIST_H
