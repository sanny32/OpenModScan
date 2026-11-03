#ifndef DIALOGUSERMSG_H
#define DIALOGUSERMSG_H

#include <QModbusRequest>
#include "qadjustedsizedialog.h"
#include "modbusclient.h"
#include "modbusmessage.h"
#include "displaydefinition.h"
#include "enums.h"

namespace Ui {
class DialogUserMsg;
}

class DialogUserMsg : public QAdjustedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogUserMsg(const DisplayDefinition& dd, QModbusPdu::FunctionCode func, DataDisplayMode mode, ModbusClient& client, QWidget *parent = nullptr);
    ~DialogUserMsg();

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_lineEditSlaveAddress_valueChanged(const QVariant&);
    void on_comboBoxFunction_functionCodeChanged(QModbusPdu::FunctionCode);
    void on_sendData_valueChanged(const QByteArray&);
    void on_pushButtonGenerate_clicked();
    void on_radioButtonHex_clicked(bool checked);
    void on_radioButtonDecimal_clicked(bool checked);
    void on_pushButtonSend_clicked();

    void on_modbusReply(const ModbusReply* const reply);
    void on_modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void on_modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg);

private:
    void updateRequestInfo();

private:
    Ui::DialogUserMsg *ui;
    QSharedPointer<const ModbusMessage> _mm;
    ModbusClient& _modbusClient;
};

#endif // DIALOGUSERMSG_H
