#ifndef DIALOGFORCEMULTIPLEREGISTERS_H
#define DIALOGFORCEMULTIPLEREGISTERS_H

#include <QTableWidgetItem>
#include "qadjustedsizedialog.h"
#include "numericlineedit.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogForceMultipleRegisters;
}

///
/// \brief The DialogForceMultipleRegisters class
///
class DialogForceMultipleRegisters : public QAdjustedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogForceMultipleRegisters(ModbusWriteParams& params, int length, bool hexAddress, QWidget *parent = nullptr);
    ~DialogForceMultipleRegisters();

    void accept() override;

private slots:
    void on_pushButton0_clicked();
    void on_pushButtonRandom_clicked();
    void on_pushButtonValue_clicked();
    void on_pushButtonSub1_clicked();
    void on_pushButtonAdd1_clicked();
    void on_pushButtonAdd2_clicked();
    void on_pushButtonAdd3_clicked();
    void on_pushButtonAdd4_clicked();
    void on_pushButtonImport_clicked();
    void on_pushButtonExport_clicked();

private:
    enum class ValueOperation {
        Set,
        Add,
        Subtract,
        Multiply,
        Divide
    };

    template<typename T>
    void applyValue(T value, int index, ValueOperation op)
    {
        switch(op)
        {
            case ValueOperation::Set:       _data[index] = value; break;
            case ValueOperation::Add:       _data[index] += value; break;
            case ValueOperation::Subtract:  _data[index] -= value; break;
            case ValueOperation::Multiply:  _data[index] *= value; break;
            case ValueOperation::Divide:    _data[index] /= value; break;
        }
    }
    void applyToAll(ValueOperation op, double value);

    void updateTableWidget();
    QLineEdit* createLineEdit();
    NumericLineEdit* createNumEdit(int idx);

private:
    Ui::DialogForceMultipleRegisters *ui;
    QVector<quint16> _data;
    ModbusWriteParams& _writeParams;
    bool _hexAddress = false;
};

#endif // DIALOGFORCEMULTIPLEREGISTERS_H
