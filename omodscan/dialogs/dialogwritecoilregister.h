#ifndef DIALOGWRITECOILREGISTER_H
#define DIALOGWRITECOILREGISTER_H

#include "qfixedsizedialog.h"
#include "modbuswriteparams.h"
#include "modbussimulationparams.h"

namespace Ui {
class DialogWriteCoilRegister;
}

///
/// \brief The DialogWriteCoilRegister class
///
class DialogWriteCoilRegister : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogWriteCoilRegister(ModbusWriteParams& params, ModbusSimulationParams& simParams, bool hexAddress, QWidget *parent = nullptr);
    ~DialogWriteCoilRegister();

    void accept() override;

private slots:
    void on_pushButtonSimulation_clicked();

private:
    Ui::DialogWriteCoilRegister *ui;

private:
    ModbusWriteParams& _writeParams;
    ModbusSimulationParams& _simParams;
};

#endif // DIALOGWRITECOILREGISTER_H
