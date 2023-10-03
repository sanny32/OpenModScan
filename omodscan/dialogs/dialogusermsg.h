#ifndef DIALOGUSERMSG_H
#define DIALOGUSERMSG_H

#include <QModbusRequest>
#include <QDialog>
#include "modbusclient.h"
#include "enums.h"

namespace Ui {
class DialogUserMsg;
}

class DialogUserMsg : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUserMsg(quint8 slaveAddres, QModbusPdu::FunctionCode func, DataDisplayMode mode, ModbusClient& client, QWidget *parent = nullptr);
    ~DialogUserMsg();

    void accept() override;

private slots:
    void on_modbusReply(QModbusReply* reply);
    void on_radioButtonHex_clicked(bool checked);
    void on_radioButtonDecimal_clicked(bool checked);

private:
    Ui::DialogUserMsg *ui;
    ModbusClient& _modbusClient;
};

#endif // DIALOGUSERMSG_H
