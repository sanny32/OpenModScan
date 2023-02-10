#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <QLabel>
#include <QStatusBar>
#include <QMdiArea>
#include "modbusclient.h"

///
/// \brief The MainStatusBar class
///
class MainStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    MainStatusBar(const ModbusClient& client, QMdiArea* parent);

    void updateNumberOfPolls();
    void updateValidSlaveResponses();
    void updateConnectionInfo(const ConnectionDetails& cd, bool connecting);

protected:
    void changeEvent(QEvent* event) override;

private:
    QMdiArea* _mdiArea;
    QLabel* _labelPolls;
    QLabel* _labelResps;
    QLabel* _labelConnectionDetails;
};

#endif // MAINSTATUSBAR_H
