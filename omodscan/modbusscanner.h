#ifndef MODBUSSCANNER_H
#define MODBUSSCANNER_H

#include <QTimer>
#include <QModbusRequest>
#include <qrange.h>
#include "connectiondetails.h"

struct ScanParams
{
    int Timeout = 1000;
    QRange<int> DeviceIds = {1, 10};
    QList<ConnectionDetails> ConnParams;
};

///
/// \brief The ModbusScanner class
///
class ModbusScanner : public QObject
{
    Q_OBJECT
public:
    explicit ModbusScanner(QObject *parent = nullptr);

    virtual void startScan();
    virtual void stopScan();
    virtual bool inProgress() const;

    QModbusRequest modbusRequest() const;
    void setModbusRequest(const QModbusRequest& req);

signals:
    void finished();
    void timeout(quint64 time);
    void found(const ConnectionDetails& cd, int deviceId);
    void progress(const ConnectionDetails& cd, int deviceId, int progress);
    void errorOccurred(const QString& error);

private slots:
    void on_timeout();

protected:
    quint64 _scanTime = 0;
    bool _inProgress = false;

    QTimer _timer;
    QModbusRequest _request;
};

#endif // MODBUSSCANNER_H
