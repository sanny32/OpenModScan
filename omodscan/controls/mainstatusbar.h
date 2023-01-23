#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <QLabel>
#include <QStatusBar>
#include <QMdiArea>

///
/// \brief The MainStatusBar class
///
class MainStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    MainStatusBar(QMdiArea* parent);

    void updateNumberOfPolls();
    void updateValidSlaveResponses();

private:
    QMdiArea* _mdiArea;
    QLabel* _labelPolls;
    QLabel* _labelResps;
};

#endif // MAINSTATUSBAR_H
