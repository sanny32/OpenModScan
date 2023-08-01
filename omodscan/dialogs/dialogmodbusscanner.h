#ifndef DIALOGMODBUSSCANNER_H
#define DIALOGMODBUSSCANNER_H

#include <QTimer>
#include <QListWidgetItem>
#include "modbusscanner.h"
#include "qfixedsizedialog.h"
#include "connectiondetails.h"

namespace Ui {
class DialogModbusScanner;
}

///
/// \brief The DialogRtuScanner class
///
class DialogModbusScanner : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogModbusScanner(QWidget *parent = nullptr);
    ~DialogModbusScanner();

protected:
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* e) override;

signals:
    void attemptToConnect(const ConnectionDetails& params, int deviceId);

private slots:
    void on_awake();
    void on_scanFinished();
    void on_timeout(quint64 time);
    void on_errorOccurred(const QString& error);
    void on_deviceFound(const ConnectionDetails& cd, int deviceId);
    void on_progress(const ConnectionDetails& cd, int deviceId, int progress);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_lineEditSubnetMask_textChanged(const QString& text);
    void on_pushButtonScan_clicked();
    void on_pushButtonClear_clicked();
    void on_radioButtonRTU_clicked();
    void on_radioButtonTCP_clicked();

private:
    void startScan();
    void stopScan();

    void clearScanTime();
    void clearProgress();

    ScanParams createSerialParams() const;
    ScanParams createTcpParams() const;

private:
    Ui::DialogModbusScanner *ui;
    QScopedPointer<ModbusScanner> _scanner;
};

#endif // DIALOGMODBUSSCANNER_H
