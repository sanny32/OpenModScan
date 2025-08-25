#include <QtMath>
#include <QLineEdit>
#include "formatutils.h"
#include "dialogforcemultiplecoils.h"
#include "ui_dialogforcemultiplecoils.h"

///
/// \brief DialogForceMultipleCoils::DialogForceMultipleCoils
/// \param params
/// \param length
/// \param hexAddress
/// \param parent
///
DialogForceMultipleCoils::DialogForceMultipleCoils(ModbusWriteParams& params, int length, bool hexAddress, QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::DialogForceMultipleCoils)
    ,_writeParams(params)
    ,_hexAddress(hexAddress)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->labelAddress->setText(QString(tr("Address: <b>%1</b>")).arg(formatAddress(QModbusDataUnit::Coils, params.Address, _hexAddress)));
    ui->labelLength->setText(QString(tr("Length: <b>%1</b>")).arg(length, 3, 10, QLatin1Char('0')));
    ui->labelSlaveDevice->setText(QString(tr("Slave Device: <b>%1</b>")).arg(params.Node, 2, 10, QLatin1Char('0')));

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
    const int columns = 8;
    const auto length = _data.length();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(columns);
    ui->tableWidget->setRowCount(qCeil(length / (double)columns));

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        const auto text = QString("+%1").arg(i);
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(text));
    }

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        const auto addressFrom = formatAddress(QModbusDataUnit::Coils, _writeParams.Address + i * columns, _hexAddress);
        const auto addressTo = formatAddress(QModbusDataUnit::Coils, _writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), _hexAddress);
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(addressFrom, addressTo)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            if(idx < length)
            {
                auto item = new QTableWidgetItem(QString::number(_data[idx]));
                item->setData(Qt::UserRole, idx);
                item->setTextAlignment(Qt::AlignCenter);
                item->setToolTip(formatAddress(QModbusDataUnit::Coils,_writeParams.Address + idx, _hexAddress));
                ui->tableWidget->setItem(i, j, item);
            }
            else
            {
                auto lineEdit = new QLineEdit(ui->tableWidget);
                lineEdit->setText("-");
                lineEdit->setFrame(false);
                lineEdit->setMaximumWidth(40);
                lineEdit->setEnabled(false);
                lineEdit->setAlignment(Qt::AlignCenter);
                ui->tableWidget->setCellWidget(i, j, lineEdit);
            }
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}
