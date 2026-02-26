#ifndef DIALOGFORCEMULTIPLECOILS_H
#define DIALOGFORCEMULTIPLECOILS_H

#include <QTableWidgetItem>
#include "modbuswriteparams.h"
#include "qadjustedsizedialog.h"

namespace Ui {
class DialogForceMultipleCoils;
}

class DialogForceMultipleCoils : public QAdjustedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogForceMultipleCoils(ModbusWriteParams& params, int length, bool hexAddress, QWidget *parent = nullptr);
    ~DialogForceMultipleCoils();

    void accept() override;

private slots:
    void on_pushButton0_clicked();
    void on_pushButton1_clicked();
    void on_pushButtonRandom_clicked();
    void on_pushButtonImport_clicked();
    void on_pushButtonExport_clicked();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    void updateTableWidget();

private:
    Ui::DialogForceMultipleCoils *ui;
    QVector<quint16> _data;
    ModbusWriteParams& _writeParams;
    bool _hexAddress;
};

#endif // DIALOGFORCEMULTIPLECOILS_H
