#ifndef RECENTFILEACTIONLIST_H
#define RECENTFILEACTIONLIST_H

#include <QMenu>

class RecentFileActionList : public QObject
{
    Q_OBJECT

public:
    RecentFileActionList(QMenu* menu, QAction* holder, QObject* parent = nullptr);

    bool isEmpty() const;

    void addRecentFile(const QString& filename);
    void removeRecentFile(const QString& filename);

signals:
    void triggered(const QString& filename);

private:
    void updateMenu();

private:
    QMenu* _menu;
    QAction* _holder;
    QAction* _insert;
    QList<QAction*> _actionList;
};

#endif // RECENTFILEACTIONLIST_H
