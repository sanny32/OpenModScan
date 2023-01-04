#ifndef DIALOGFORCEMULTIPLECOILS_H
#define DIALOGFORCEMULTIPLECOILS_H

#include <QDialog>
#include "displaydefinition.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogForceMultipleCoils;
}

class DialogForceMultipleCoils : public QDialog
{
    Q_OBJECT

public:
    explicit DialogForceMultipleCoils(ModbusWriteParams& params, int length, QWidget *parent = nullptr);
    ~DialogForceMultipleCoils();

    void accept() override;

private:
    Ui::DialogForceMultipleCoils *ui;
    ModbusWriteParams& _writeParams;
};

#endif // DIALOGFORCEMULTIPLECOILS_H
