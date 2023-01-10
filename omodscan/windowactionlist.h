#ifndef WINDOWACTIONLIST_H
#define WINDOWACTIONLIST_H

#include <QMenu>
#include <QMdiSubWindow>

///
/// \brief The WindowActionList class
///
class WindowActionList : public QObject
{
    Q_OBJECT
public:
    explicit WindowActionList(QMenu* menu, QObject *parent = nullptr);

    bool isEmpty() const;

    void addWindow(QMdiSubWindow* wnd);
    void removeWindow(QMdiSubWindow* wnd);

    void update();

signals:
    void triggered(QMdiSubWindow* wnd);

private:
    void updateMenu();

private:
    QMenu* _menu;
    QList<QAction*> _actionList;
};

#endif // WINDOWACTIONLIST_H
