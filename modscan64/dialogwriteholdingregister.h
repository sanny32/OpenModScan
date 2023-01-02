#ifndef DIALOGWRITEHOLDINGREGISTER_H
#define DIALOGWRITEHOLDINGREGISTER_H

#include <QDialog>
#include "enums.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogWriteHoldingRegister;
}

///
/// \brief The DialogWriteHoldingRegister class
///
class DialogWriteHoldingRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWriteHoldingRegister(const ModbusWriteParams& params, DataDisplayMode mode, QWidget *parent = nullptr);
    ~DialogWriteHoldingRegister();

    ModbusWriteParams writeParams() const;

private:
    Ui::DialogWriteHoldingRegister *ui;
};

#endif // DIALOGWRITEHOLDINGREGISTER_H
