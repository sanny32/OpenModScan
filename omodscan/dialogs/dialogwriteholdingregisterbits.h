#ifndef DIALOGWRITEHOLDINGREGISTERBITS_H
#define DIALOGWRITEHOLDINGREGISTERBITS_H

#include "qfixedsizedialog.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogWriteHoldingRegisterBits;
}

class DialogWriteHoldingRegisterBits : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogWriteHoldingRegisterBits(ModbusWriteParams& params, bool hexAddress, QWidget *parent = nullptr);
    ~DialogWriteHoldingRegisterBits();

    void accept() override;

private:
    Ui::DialogWriteHoldingRegisterBits *ui;
    ModbusWriteParams& _writeParams;
};

#endif // DIALOGWRITEHOLDINGREGISTERBITS_H
