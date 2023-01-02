#ifndef DIALOGWRITEHOLDINGREGISTER_H
#define DIALOGWRITEHOLDINGREGISTER_H

#include <QDialog>
#include <QVariant>
#include <QModbusClient>
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
    explicit DialogWriteHoldingRegister(const ModbusWriteParams& params, DisplayMode mode, QWidget *parent = nullptr);
    ~DialogWriteHoldingRegister();

    ModbusWriteParams writeParams() const;

private:
    Ui::DialogWriteHoldingRegister *ui;

private:
    QModbusClient* _modbusClient;
};

#endif // DIALOGWRITEHOLDINGREGISTER_H
