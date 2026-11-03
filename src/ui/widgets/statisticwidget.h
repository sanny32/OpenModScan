#ifndef STATISTICWIDGET_H
#define STATISTICWIDGET_H

#include <QWidget>
#include "enums.h"

namespace Ui {
class StatisticWidget;
}

///
/// \brief The StatisticWidget class
///
class StatisticWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticWidget(QWidget *parent = nullptr);
    ~StatisticWidget();

    uint numberOfPolls() const { return _numberOfPolls; }
    uint validSlaveResposes() const { return _validSlaveResponses; }

    void increaseNumberOfPolls();
    void increaseValidSlaveResponses();
    void resetCtrs();

    PollState pollState() const;
    void setPollState(PollState state);

signals:
    void numberOfPollsChanged(uint value);
    void validSlaveResposesChanged(uint value);
    void ctrsReseted();
    void pollStateChanged(PollState state);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_pushButtonResetCtrs_clicked();
    void on_pushButtonPause_clicked();

private:
    void updateStatistic();

private:
    Ui::StatisticWidget *ui;

private:
    uint _numberOfPolls;
    uint _validSlaveResponses;
    PollState _pollState;
};

#endif // STATISTICWIDGET_H
