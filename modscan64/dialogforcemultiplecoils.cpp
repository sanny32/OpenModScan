#include "dialogforcemultiplecoils.h"
#include "ui_dialogforcemultiplecoils.h"

///
/// \brief DialogForceMultipleCoils::DialogForceMultipleCoils
/// \param params
/// \param length
/// \param parent
///
DialogForceMultipleCoils::DialogForceMultipleCoils(ModbusWriteParams& params, int length, QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::DialogForceMultipleCoils)
    ,_writeParams(params)
{
    ui->setupUi(this);
    ui->labelAddress->setText(QStringLiteral("Address: %1").arg(params.Address, 5, 10, QLatin1Char('0')));
    ui->labelLength->setText(QStringLiteral("Length: %1").arg(length, 3, 10, QLatin1Char('0')));

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setRowCount(50 / 8 + 1);

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        const auto text = QString("+%1").arg(i);
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(text));
    }

    const auto columns = ui->tableWidget->columnCount();
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        const auto address1Text = QString("%1").arg(params.Address + i * columns, 4, 10, QLatin1Char('0'));
        const auto address2Text = QString("%1").arg(params.Address + qMin(length - 1, (i + 1) * columns - 1), 4, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(address1Text, address2Text)));

        for(int j = 0; j < columns; j++)
        {
            const auto text = (i * columns + j < length) ? "0" : "-";
            auto item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);

            ui->tableWidget->setItem(i, j, item);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

///
/// \brief DialogForceMultipleCoils::~DialogForceMultipleCoils
///
DialogForceMultipleCoils::~DialogForceMultipleCoils()
{
    delete ui;
}

///
/// \brief DialogForceMultipleCoils::accept
///
void DialogForceMultipleCoils::accept()
{
    QDialog::accept();
}
