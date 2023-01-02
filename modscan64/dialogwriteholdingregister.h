#ifndef DIALOGWRITEHOLDINGREGISTER_H
#define DIALOGWRITEHOLDINGREGISTER_H

#include <QDialog>
#include <QVariant>
#include <QModbusClient>
#include "enums.h"

namespace Ui {
class DialogWriteHoldingRegister;
}

struct WriteRegisterParams
{
    quint32 Node;
    quint32 Address;
    QVariant Value;
    DisplayMode Mode;
};

///
/// \brief The DialogWriteHoldingRegister class
///
class DialogWriteHoldingRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWriteHoldingRegister(const WriteRegisterParams& params, QModbusClient* client, QWidget *parent = nullptr);
    ~DialogWriteHoldingRegister();

private:
    Ui::DialogWriteHoldingRegister *ui;

private:
    QModbusClient* _modbusClient;
};

#endif // DIALOGWRITEHOLDINGREGISTER_H
