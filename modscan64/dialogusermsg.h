#ifndef DIALOGUSERMSG_H
#define DIALOGUSERMSG_H

#include "qfixedsizedialog.h"
#include "enums.h"

namespace Ui {
class DialogUserMsg;
}

class DialogUserMsg : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogUserMsg(quint8 slaveAddres, DataDisplayMode mode, QWidget *parent = nullptr);
    ~DialogUserMsg();

    void accept() override;

private slots:
    void on_radioButtonHex_clicked(bool checked);
    void on_radioButtonDecimal_clicked(bool checked);

private:
    Ui::DialogUserMsg *ui;
};

#endif // DIALOGUSERMSG_H
