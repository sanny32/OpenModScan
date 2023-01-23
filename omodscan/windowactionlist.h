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
    explicit WindowActionList(QMenu* menu, QAction* placeHolder);

    bool isEmpty() const;
    QList<QAction*> actionList() const;

    void addWindow(QMdiSubWindow* wnd);
    void removeWindow(QMdiSubWindow* wnd);

    void update();

signals:
    void triggered(QMdiSubWindow* wnd);

private:
    void updateMenu();

private:
    QMenu* _menu;
    QAction* _placeHolder;
    QList<QAction*> _actionList;
};

#endif // WINDOWACTIONLIST_H
