#ifndef DIALOGWRITEHOLDINGREGISTER_H
#define DIALOGWRITEHOLDINGREGISTER_H

#include "enums.h"
#include "modbuswriteparams.h"
#include "modbussimulationparams.h"
#include "qfixedsizedialog.h"
#include "checkablegroupbox.h"
#include "bitpatterncontrol.h"

class DataSimulator;

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
    explicit DialogWriteHoldingRegister(ModbusWriteParams& params, bool hexAddress,
                                        DataSimulator* dataSimulator = nullptr,
                                        QWidget *parent = nullptr);
    ~DialogWriteHoldingRegister();

    void accept() override;

private slots:
    void on_pushButtonSimulation_clicked();
    void on_lineEditAddress_valueChanged(const QVariant& value);
    void on_lineEditNode_valueChanged(const QVariant& value);

private:
    void updateSimulationButton();
    void updateValue();

private:
    Ui::DialogWriteHoldingRegister *ui;

private:
    ModbusWriteParams& _writeParams;
    ModbusSimulationParams _simParams;
    DataSimulator* _dataSimulator;
};

#endif // DIALOGWRITEHOLDINGREGISTER_H
