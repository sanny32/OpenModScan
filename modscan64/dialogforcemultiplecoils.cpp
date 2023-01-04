#include "dialogforcemultiplecoils.h"
#include "ui_dialogforcemultiplecoils.h"

///
/// \brief DialogForceMultipleCoils::DialogForceMultipleCoils
/// \param params
/// \param parent
///
DialogForceMultipleCoils::DialogForceMultipleCoils(ModbusWriteParams& params, int length, QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::DialogForceMultipleCoils)
    ,_writeParams(params)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->labelAddress->setText(QStringLiteral("Address: %1").arg(params.Address, 5, 10, QLatin1Char('0')));
    ui->labelLength->setText(QStringLiteral("Length: %1").arg(length, 3, 10, QLatin1Char('0')));

    _data = params.Value.value<QVector<quint16>>();
    if(_data.length() != length) _data.resize(length);

    updateTableWidget();
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
    _writeParams.Value = QVariant::fromValue(_data);
    QDialog::accept();
}

///
/// \brief DialogForceMultipleCoils::on_pushButton0_clicked
///
void DialogForceMultipleCoils::on_pushButton0_clicked()
{
    for(auto& v : _data)
    {
        v = 0;
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleCoils::on_pushButton1_clicked
///
void DialogForceMultipleCoils::on_pushButton1_clicked()
{
    for(auto& v : _data)
    {
        v = 1;
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleCoils::on_tableWidget_itemDoubleClicked
/// \param item
///
void DialogForceMultipleCoils::on_tableWidget_itemDoubleClicked(QTableWidgetItem* item)
{
    if(item == nullptr) return;

    bool ok;
    const auto idx = item->data(Qt::UserRole).toInt(&ok);
    if(ok)
    {
        _data[idx] = !_data[idx];
        item->setText(QString::number(_data[idx]));
    }
}

///
/// \brief DialogForceMultipleCoils::updateTableWidget
///
void DialogForceMultipleCoils::updateTableWidget()
{
    const auto columns = 8;
    const auto length = _data.length();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(columns);
    ui->tableWidget->setRowCount(ceil(length / (double)columns));

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        const auto text = QString("+%1").arg(i);
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(text));
    }

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        const auto address1Text = QString("%1").arg(_writeParams.Address + i * columns, 4, 10, QLatin1Char('0'));
        const auto address2Text = QString("%1").arg(_writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), 4, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(address1Text, address2Text)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            const auto text = (idx < length) ? QString::number(_data[idx]) : "-";

            auto item = new QTableWidgetItem(text);

            item->setTextAlignment(Qt::AlignCenter);
            if(idx < length)
            {
                item->setData(Qt::UserRole, idx);
                item->setToolTip(QString("%1").arg(_writeParams.Address + idx, 4, 10, QLatin1Char('0')));
            }

            ui->tableWidget->setItem(i, j, item);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}
