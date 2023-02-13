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
    explicit MainStatusBar(const ModbusClient& client, QMdiArea* parent);
    ~MainStatusBar();

    void updateNumberOfPolls();
    void updateValidSlaveResponses();

protected:
    void changeEvent(QEvent* event) override;

private:
    void updateConnectionInfo(const ConnectionDetails& cd, bool connecting);

private:
    QMdiArea* _mdiArea;
    QLabel* _labelPolls;
    QLabel* _labelResps;
    QLabel* _labelConnectionDetails;
};

#endif // MAINSTATUSBAR_H
