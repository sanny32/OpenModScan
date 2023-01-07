#ifndef DIALOGFORCEMULTIPLEREGISTERS_H
#define DIALOGFORCEMULTIPLEREGISTERS_H

#include <QDialog>
#include <QTableWidgetItem>
#include "numericlineedit.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogForceMultipleRegisters;
}

///
/// \brief The DialogForceMultipleRegisters class
///
class DialogForceMultipleRegisters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogForceMultipleRegisters(ModbusWriteParams& params, int length, QWidget *parent = nullptr);
    ~DialogForceMultipleRegisters();

    void accept() override;

private slots:
    void on_pushButton0_clicked();
    void on_pushButtonRandom_clicked();

private:
    void updateTableWidget();
    QLineEdit* createLineEdit();
    NumericLineEdit* createNumEdit(int idx);

private:
    Ui::DialogForceMultipleRegisters *ui;
    QVector<quint16> _data;
    ModbusWriteParams& _writeParams;
};

#endif // DIALOGFORCEMULTIPLEREGISTERS_H
