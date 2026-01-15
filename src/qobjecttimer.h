#ifndef QOBJECTTIMER_H
#define QOBJECTTIMER_H

#include <QObject>
#include <QTimerEvent>

///
/// \brief The QObjectTimer class
///
class QObjectTimer : public QObject
{
    Q_OBJECT

public:
    QObjectTimer() = default;
    int start(int msec)
    {
        _timer = QBasicTimer();
        _timer.start(msec, Qt::PreciseTimer, this);
        return _timer.timerId();
    }
    void stop() { _timer.stop(); }
    bool isActive() const { return _timer.isActive(); }

signals:
    void timeout(int timerId);

private:
    void timerEvent(QTimerEvent *event) override
    {
        const auto id = _timer.timerId();
        if (event->timerId() == id)
            emit timeout(id);
    }

private:
    QBasicTimer _timer;
};

#endif // QOBJECTTIMER_H
