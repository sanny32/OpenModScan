#include <QMdiSubWindow>
#include "formmodsca.h"
#include "mainstatusbar.h"

///
/// \brief MainStatusBar::MainStatusBar
/// \param parent
///
MainStatusBar::MainStatusBar(QMdiArea* parent)
    : QStatusBar(parent)
    ,_mdiArea(parent)
{
    Q_ASSERT(_mdiArea != nullptr);

    _labelPolls = new QLabel(this);
    _labelPolls->setText(QString(tr("Polls: %1")).arg(0));
    _labelPolls->setFrameShadow(QFrame::Sunken);
    _labelPolls->setFrameShape(QFrame::Panel);
    _labelPolls->setMinimumWidth(120);

    _labelResps = new QLabel(this);
    _labelResps->setText(QString(tr("Resps: %1")).arg(0));
    _labelResps->setFrameShadow(QFrame::Sunken);
    _labelResps->setFrameShape(QFrame::Panel);
    _labelResps->setMinimumWidth(120);

    addPermanentWidget(_labelPolls);
    addPermanentWidget(_labelResps);
}

///
/// \brief MainStatusBar::updateNumberOfPolls
///
void MainStatusBar::updateNumberOfPolls()
{
    uint polls = 0;
    for(auto&& wnd : _mdiArea->subWindowList())
    {
        const auto frm = dynamic_cast<FormModSca*>(wnd->widget());
        if(frm) polls += frm->numberOfPolls();
    }

    _labelPolls->setText(QString(tr("Polls: %1")).arg(polls));
}

///
/// \brief MainStatusBar::updateValidSlaveResponses
///
void MainStatusBar::updateValidSlaveResponses()
{
    uint resps = 0;
    for(auto&& wnd : _mdiArea->subWindowList())
    {
        const auto frm = dynamic_cast<FormModSca*>(wnd->widget());
        if(wnd) resps += frm->validSlaveResposes();
    }

    _labelResps->setText(QString(tr("Resps: %1")).arg(resps));
}
