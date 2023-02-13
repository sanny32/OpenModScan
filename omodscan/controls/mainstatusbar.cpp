#include <QEvent>
#include <QMdiSubWindow>
#include "formmodsca.h"
#include "mainstatusbar.h"

///
/// \brief Parity_toString
/// \param parity
/// \return
///
QString Parity_toString(QSerialPort::Parity parity)
{
    switch(parity)
    {
        case QSerialPort::NoParity:
        return MainStatusBar::tr("NONE");

        case QSerialPort::EvenParity:
        return MainStatusBar::tr("EVEN");

        case QSerialPort::OddParity:
        return MainStatusBar::tr("ODD");

        case QSerialPort::SpaceParity:
        return MainStatusBar::tr("SPACE");

        case QSerialPort::MarkParity:
        return MainStatusBar::tr("MARK");

        default:
        break;
    }

    return QString();
}

///
/// \brief MainStatusBar::MainStatusBar
/// \param client
/// \param parent
///
MainStatusBar::MainStatusBar(const ModbusClient& client, QMdiArea* parent)
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

    _labelConnectionDetails = new QLabel(this);
    _labelConnectionDetails->setVisible(false);

    addWidget(_labelConnectionDetails);

    connect(&client, &ModbusClient::modbusConnecting, this, [&](const ConnectionDetails& cd)
    {
        updateConnectionInfo(cd, true);
    });

    connect(&client, &ModbusClient::modbusConnected, this, [&](const ConnectionDetails& cd)
    {
        updateConnectionInfo(cd, false);
    });

    connect(&client, &ModbusClient::modbusDisconnected, this, [&](const ConnectionDetails&)
    {
        _labelConnectionDetails->setText(QString());
        _labelConnectionDetails->setVisible(false);
    });
}

///
/// \brief MainStatusBar::changeEvent
/// \param event
///
void MainStatusBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
       updateNumberOfPolls();
       updateValidSlaveResponses();

       if(_labelConnectionDetails->isVisible())
       {
            const bool connecting = _labelConnectionDetails->property("Connecting").toBool();
            const auto cd = _labelConnectionDetails->property("ConnectionDetails").value<ConnectionDetails>();
            updateConnectionInfo(cd, connecting);
       }
    }

    QStatusBar::changeEvent(event);
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


///
/// \brief MainStatusBar::updateConnectionInfo
/// \param cd
/// \param connecting
///
void MainStatusBar::updateConnectionInfo(const ConnectionDetails& cd, bool connecting)
{
    _labelConnectionDetails->setProperty("Connecting", connecting);
    _labelConnectionDetails->setProperty("ConnectionDetails", QVariant::fromValue(cd));

    QString info;
    switch(cd.Type)
    {
        case ConnectionType::Tcp:
            info = connecting ? QString(tr("Connecting to %1:%2...  ").arg(cd.TcpParams.IPAddress, QString::number(cd.TcpParams.ServicePort))) :
                                QString(tr("Remote TCP/IP Server %1:%2  ")).arg(cd.TcpParams.IPAddress, QString::number(cd.TcpParams.ServicePort));
        break;
        case ConnectionType::Serial:
            info = connecting ? QString(tr("Connecting to %1...  ").arg(cd.SerialParams.PortName)) :
                                QString(tr("Port %1:%2:%3:%4:%5  ")).arg(
                                cd.SerialParams.PortName,
                                QString::number(cd.SerialParams.BaudRate),
                                QString::number(cd.SerialParams.WordLength),
                                Parity_toString(cd.SerialParams.Parity),
                                QString::number(cd.SerialParams.StopBits));
        break;
    }

    _labelConnectionDetails->setText(info);
    _labelConnectionDetails->setVisible(true);
}
