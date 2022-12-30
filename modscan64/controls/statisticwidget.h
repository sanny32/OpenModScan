#ifndef STATISTICWIDGET_H
#define STATISTICWIDGET_H

#include <QWidget>

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

    void increaseNumberOfPolls();
    void increaseValidSlaveResponses();
    void resetCtrs();

private slots:
    void on_pushButtonResetCtrs_clicked();

private:
    void updateStatistic();

private:
    Ui::StatisticWidget *ui;

private:
    uint _numberOfPolls;
    uint _validSlaveResponses;
};

#endif // STATISTICWIDGET_H
