#include <QtMath>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QRandomGenerator>
#include "uiutils.h"
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
DialogForceMultipleCoils::DialogForceMultipleCoils(ModbusWriteParams& params, int length, bool hexAddress, QWidget *parent)
    : QAdjustedSizeDialog(parent)
    , ui(new Ui::DialogForceMultipleCoils)
    ,_writeParams(params)
    ,_hexAddress(hexAddress)
{
    ui->setupUi(this);

    ui->labelAddress->setText(QString(ui->labelAddress->text()).arg(formatAddress(QModbusDataUnit::Coils, params.Address, params.AddrSpace, _hexAddress)));
    ui->labelLength->setText(QString(ui->labelLength->text()).arg(length, 3, 10, QLatin1Char('0')));
    ui->labelSlaveDevice->setText(QString(ui->labelSlaveDevice->text()).arg(params.DeviceId, 3, 10, QLatin1Char('0')));
    ui->labelAddresses->setText(QString(ui->labelAddresses->text()).arg(
        formatAddress(QModbusDataUnit::Coils, params.Address, params.AddrSpace, _hexAddress),
        formatAddress(QModbusDataUnit::Coils, params.Address + length - 1, params.AddrSpace, _hexAddress)));

    recolorPushButtonIcon(ui->pushButtonExport, Qt::red);
    recolorPushButtonIcon(ui->pushButtonImport, Qt::darkGreen);

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
/// \brief DialogForceMultipleCoils::on_pushButtonRandom_clicked
///
void DialogForceMultipleCoils::on_pushButtonRandom_clicked()
{
    for(auto& v : _data)
    {
        v = (QRandomGenerator::global()->bounded(0, 2) != 0);
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleCoils::on_pushButtonImport_clicked
///
void DialogForceMultipleCoils::on_pushButtonImport_clicked()
{
    auto filename = QFileDialog::getOpenFileName(this, QString(), QString(), tr("CSV files (*.csv)"));
    if(filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), file.errorString());
        return;
    }

    QTextStream ts(&file);

    QVector<quint16> newData;
    bool headerSkipped = false;

    while(!ts.atEnd())
    {
        QString line = ts.readLine().trimmed();
        if(line.isEmpty()) {
            continue;
        }

        if(!headerSkipped)
        {
            headerSkipped = true;
            continue;
        }

        const QStringList parts = line.split(";");
        if(parts.size() < 2) {
            continue;
        }

        const auto valueStr = parts[1].trimmed();

        bool ok = false;
        const quint16 value = (valueStr.toUShort(&ok, 10) != 0);

        if(!ok)
        {
            QMessageBox::warning(this, tr("Import error"), tr("Invalid value: %1").arg(valueStr));
            return;
        }

        newData.append(value);
    }

    if(newData.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("No data found in file."));
        return;
    }

    if(newData.size() != _data.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Imported data size (%1) does not match current size (%2).").arg(newData.size()).arg(_data.size()));
    }

    for(int i = 0; i < _data.size(); ++i) {
        if(i < newData.size()) {
            _data[i] = newData[i];
        }
    }

    updateTableWidget();

}

///
/// \brief DialogForceMultipleCoils::on_pushButtonExport_clicked
///
void DialogForceMultipleCoils::on_pushButtonExport_clicked()
{
    auto filename = QFileDialog::getSaveFileName(this, QString(), QString(), tr("CSV files (*.csv)"));
    if(filename.isEmpty()) return;

    if(!filename.endsWith(".csv", Qt::CaseInsensitive))
    {
        filename += ".csv";
    }

    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), file.errorString());
        return;
    }

    QTextStream ts(&file);
    ts.setGenerateByteOrderMark(true);

    const char* delim = ";";
    ts << "Address" << delim << "Value" << "\n";

    for(int i = 0; i < _data.size(); i++)
    {
        ts << formatAddress(QModbusDataUnit::HoldingRegisters, _writeParams.Address + i, _writeParams.AddrSpace, _hexAddress)
        << delim
        << QString::number(_data[i])
        << "\n";
    }
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
        const auto addressFrom = formatAddress(QModbusDataUnit::Coils, _writeParams.Address + i * columns, _writeParams.AddrSpace, _hexAddress);
        const auto addressTo = formatAddress(QModbusDataUnit::Coils, _writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), _writeParams.AddrSpace, _hexAddress);
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(addressFrom, addressTo)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            if(idx < length)
            {
                auto item = new QTableWidgetItem(QString::number(_data[idx]));
                item->setData(Qt::UserRole, idx);
                item->setTextAlignment(Qt::AlignCenter);
                item->setToolTip(formatAddress(QModbusDataUnit::Coils,_writeParams.Address + idx, _writeParams.AddrSpace, _hexAddress));
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
