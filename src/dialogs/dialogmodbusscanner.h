#ifndef DIALOGMODBUSSCANNER_H
#define DIALOGMODBUSSCANNER_H

#include <QTimer>
#include <QListWidgetItem>
#include "modbusscanner.h"
#include "qfixedsizedialog.h"
#include "connectiondetails.h"
#include "modbusprotocolscombobox.h"

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
    explicit DialogModbusScanner(bool hexAddress, QWidget *parent = nullptr);
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
    void on_deviceFound(const ConnectionDetails& cd, int deviceId, bool dubious);
    void on_progress(const ConnectionDetails& cd, int deviceId, int progress);

    void on_comboBoxFunction_functionCodeChanged(QModbusPdu::FunctionCode funcCode);
    void on_comboBoxAddressBase_addressBaseChanged(AddressBase base);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_lineEditIPAddressFrom_editingFinished();
    void on_lineEditSubnetMask_editingFinished();
    void on_pushButtonScan_clicked();
    void on_pushButtonClear_clicked();
    void on_comboBoxProtocols_modbusProtocolChanged(ModbusProtocolsComboBox::ModbusProtocol mbp);

private:
    void startScan();
    void stopScan();

    void clearScanTime();
    void clearProgress();

    const ScanParams createSerialParams(TransmissionMode mode) const;
    const ScanParams createTcpParams(TransmissionMode mode) const;
    const QModbusRequest createModbusRequest() const;

private:
    Ui::DialogModbusScanner *ui;
    QModbusPdu::FunctionCode _rtuFuncCode;
    QModbusPdu::FunctionCode _tcpFuncCode;
    QScopedPointer<ModbusScanner> _scanner;
    QIcon _iconStart;
    QIcon _iconStop;
};

#endif // DIALOGMODBUSSCANNER_H
