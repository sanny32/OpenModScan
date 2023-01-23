#ifndef DIALOGWRITEHOLDINGREGISTER_H
#define DIALOGWRITEHOLDINGREGISTER_H

#include "enums.h"
#include "modbuswriteparams.h"
#include "qfixedsizedialog.h"

namespace Ui {
class DialogWriteHoldingRegister;
}

///
/// \brief The DialogWriteHoldingRegister class
///
class DialogWriteHoldingRegister : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogWriteHoldingRegister(ModbusWriteParams& params, DataDisplayMode mode, QWidget *parent = nullptr);
    ~DialogWriteHoldingRegister();

    void accept() override;

private:
    Ui::DialogWriteHoldingRegister *ui;
    ModbusWriteParams& _writeParams;
};

#endif // DIALOGWRITEHOLDINGREGISTER_H
