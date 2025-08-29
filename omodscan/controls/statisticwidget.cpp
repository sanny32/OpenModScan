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
/// \brief StatisticWidget::changeEvent
/// \param event
///
void StatisticWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

///
/// \brief StatisticWidget::increaseNumberOfPolls
///
void StatisticWidget::increaseNumberOfPolls()
{
    _numberOfPolls++;
    updateStatistic();

    emit numberOfPollsChanged(_numberOfPolls);
}

///
/// \brief StatisticWidget::increaseValidSlaveResponses
///
void StatisticWidget::increaseValidSlaveResponses()
{
   _validSlaveResponses++;
   updateStatistic();

   emit validSlaveResposesChanged(_validSlaveResponses);
}

///
/// \brief StatisticWidget::resetCtrls
///
void StatisticWidget::resetCtrs()
{
    _numberOfPolls = 0;
    _validSlaveResponses = 0;

    updateStatistic();

    emit numberOfPollsChanged(_numberOfPolls);
    emit validSlaveResposesChanged(_validSlaveResponses);
}

///
/// \brief StatisticWidget::pollState
/// \return
///
PollState StatisticWidget::pollState() const
{
    return _pollState;
}

///
/// \brief StatisticWidget::setPollState
/// \param state
///
void StatisticWidget::setPollState(PollState state)
{
    _pollState = state;
    switch (state)
    {
        case Off:
            ui->pushButtonPause->setEnabled(false);
            ui->pushButtonPause->setText(tr("Pause Poll"));
        break;
        case Running:
            ui->pushButtonPause->setEnabled(true);
            ui->pushButtonPause->setText(tr("Pause Poll"));
        break;
        case Paused:
            ui->pushButtonPause->setEnabled(true);
            ui->pushButtonPause->setText(tr("Resume Poll"));
        break;
    }
}

///
/// \brief StatisticWidget::on_pushButtonResetCtrs_clicked
///
void StatisticWidget::on_pushButtonResetCtrs_clicked()
{
    resetCtrs();
    emit ctrsReseted();
}

///
/// \brief StatisticWidget::on_pushButtonPause_clicked
///
void StatisticWidget::on_pushButtonPause_clicked()
{
    switch (_pollState) {
    case Off: break;
    case Running: setPollState(Paused); break;
    case Paused: setPollState(Running); break;
    }

    emit pollStateChanged(pollState());
}

///
/// \brief StatisticWidget::updateStatistic
///
void StatisticWidget::updateStatistic()
{
    ui->labelNumberOfPolls->setText(QString(tr("Number of Polls: %1")).arg(_numberOfPolls));
    ui->labelValidSlaveResponses->setText(QString(tr("Valid Slave Responses: %1")).arg(_validSlaveResponses));
}
