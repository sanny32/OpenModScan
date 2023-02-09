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
        return QObject::tr("NONE");

        case QSerialPort::EvenParity:
        return QObject::tr("EVEN");

        case QSerialPort::OddParity:
        return QObject::tr("ODD");

        case QSerialPort::SpaceParity:
        return QObject::tr("SPACE");

        case QSerialPort::MarkParity:
        return QObject::tr("MARK");

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
        QString info;
        switch(cd.Type)
        {
            case ConnectionType::Tcp:
                info = QString(tr("Connecting to %1:%2...  ").arg(cd.TcpParams.IPAddress,
                                                                  QString::number(cd.TcpParams.ServicePort)));
            break;

            case ConnectionType::Serial:
                info = QString(tr("Connecting to %1...  ").arg(cd.SerialParams.PortName));
            break;
        }

         _labelConnectionDetails->setText(info);
        _labelConnectionDetails->setVisible(true);
    });

    connect(&client, &ModbusClient::modbusConnected, this, [&](const ConnectionDetails& cd)
    {
        updateConnectionInfo(cd);
    });

    connect(&client, &ModbusClient::modbusDisconnected, this, [&](const ConnectionDetails&)
    {
        _labelConnectionDetails->setText(QString());
        _labelConnectionDetails->setVisible(false);
    });
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
///
void MainStatusBar::updateConnectionInfo(const ConnectionDetails& cd)
{
    switch(cd.Type)
    {
        case ConnectionType::Tcp:
            _labelConnectionDetails->setText(QString(tr("Remote TCP/IP Server %1:%2  ")).arg(cd.TcpParams.IPAddress,
                                                                              QString::number(cd.TcpParams.ServicePort)));
        break;

        case ConnectionType::Serial:
        {
            const auto info = QString(tr("Port %1:%2:%3:%4:%5  ")).arg(
                        cd.SerialParams.PortName,
                        QString::number(cd.SerialParams.BaudRate),
                        QString::number(cd.SerialParams.WordLength),
                        Parity_toString(cd.SerialParams.Parity),
                        QString::number(cd.SerialParams.StopBits));
            _labelConnectionDetails->setText(info);
        }
        break;
    }

    _labelConnectionDetails->setVisible(true);
}
