#ifndef DIALOGWRITECOILREGISTER_H
#define DIALOGWRITECOILREGISTER_H

#include "qfixedsizedialog.h"
#include "modbuswriteparams.h"
#include "modbussimulationparams.h"

class DataSimulator;

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
    explicit DialogWriteCoilRegister(ModbusWriteParams& params, bool hexAddress,
                                     DataSimulator* dataSimulator = nullptr,
                                     QWidget *parent = nullptr);
    ~DialogWriteCoilRegister();

    void accept() override;

private slots:
    void on_pushButtonSimulation_clicked();
    void on_lineEditAddress_valueChanged(const QVariant& value);
    void on_lineEditNode_valueChanged(const QVariant& value);

private:
    void updateSimulationButton();
    void updateValue();

private:
    Ui::DialogWriteCoilRegister *ui;

private:
    ModbusWriteParams& _writeParams;
    ModbusSimulationParams _simParams;
    DataSimulator* _dataSimulator;
};

#endif // DIALOGWRITECOILREGISTER_H
