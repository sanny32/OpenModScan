#ifndef DIALOGRTUSCANNER_H
#define DIALOGRTUSCANNER_H

#include <QTimer>
#include "qfixedsizedialog.h"
#include "connectiondetails.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QModbusRtuSerialMaster>
    typedef QModbusRtuSerialMaster QModbusRtuSerialClient;
#else
    #include <QModbusRtuSerialClient>
#endif

namespace Ui {
class DialogRtuScanner;
}

class DialogRtuScanner : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogRtuScanner(QWidget *parent = nullptr);
    ~DialogRtuScanner();

private slots:
    void on_awake();
    void on_timeout();
    void on_pushButtonScan_clicked();
    void on_pushButtonClear_clicked();
    void on_errorOccurred(QModbusDevice::Error error);
    void on_stateChanged(QModbusDevice::State state);

private:
    void startScan();
    void stopScan();

    void clearScanTime();
    void clearProgress();

    void printScanInfo(const SerialConnectionParams& params, int address);
    void printResult(const SerialConnectionParams& params, int address);

    void connectDevice(const SerialConnectionParams& params);
    void sendRequest(int address);

private:
    Ui::DialogRtuScanner *ui;
    QModbusRtuSerialClient* _modbusClient;

private:
    int _requestCount = 0;
    quint64 _scanTime = 0;
    bool _scanning = false;
    bool _finished = false;

    QTimer _scanTimer;
    QVector<SerialConnectionParams> _connParams;
    QVector<SerialConnectionParams>::ConstIterator _iterator;
};

#endif // DIALOGRTUSCANNER_H
