#include "statisticwidget.h"
#include "ui_statisticwidget.h"

///
/// \brief StatisticWidget::StatisticWidget
/// \param parent
///
StatisticWidget::StatisticWidget(QWidget *parent) :
      QWidget(parent)
    , ui(new Ui::StatisticWidget)
    ,_numberOfPolls(0)
    ,_validSlaveResponses(0)
{
    ui->setupUi(this);
}

///
/// \brief StatisticWidget::~StatisticWidget
///
StatisticWidget::~StatisticWidget()
{
    delete ui;
}

///
/// \brief StatisticWidget::increaseNumberOfPolls
///
void StatisticWidget::increaseNumberOfPolls()
{
    _numberOfPolls++;
    updateStatistic();
}

///
/// \brief StatisticWidget::increaseValidSlaveResponses
///
void StatisticWidget::increaseValidSlaveResponses()
{
   _validSlaveResponses++;
   updateStatistic();
}

///
/// \brief StatisticWidget::on_pushButtonResetCtrls_clicked
///
void StatisticWidget::on_pushButtonResetCtrls_clicked()
{
    _numberOfPolls = 0;
    _validSlaveResponses = 0;

    updateStatistic();
}

///
/// \brief StatisticWidget::updateStatistic
///
void StatisticWidget::updateStatistic()
{
    ui->labelNumberOfPolls->setText(QString("Number of Polls: %1").arg(_numberOfPolls));
    ui->labelValidSlaveResponses->setText(QString("Valid Slave Responses: %1").arg(_validSlaveResponses));
}
