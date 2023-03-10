#ifndef DIALOGUSERMSG_H
#define DIALOGUSERMSG_H

#include <QModbusRequest>
#include "qfixedsizedialog.h"
#include "modbusclient.h"
#include "enums.h"

namespace Ui {
class DialogUserMsg;
}

class DialogUserMsg : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogUserMsg(quint8 slaveAddres, DataDisplayMode mode, ModbusClient& client, QWidget *parent = nullptr);
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
