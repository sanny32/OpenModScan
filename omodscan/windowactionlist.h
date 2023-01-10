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
    explicit WindowActionList(QMenu* menu);

    bool isEmpty() const;

    void addWindow(QMdiSubWindow* wnd);
    void removeWindow(QMdiSubWindow* wnd);

    void update();

signals:
    void triggered(QMdiSubWindow* wnd);

private slots:
    void showWindowsDialog();

private:
    void updateMenu();

private:
    QMenu* _menu;
    QAction* _actionWindows;
    QList<QAction*> _actionList;
};

#endif // WINDOWACTIONLIST_H
