#include <QtMath>
#include <QRandomGenerator>
#include "numericlineedit.h"
#include "dialogforcemultipleregisters.h"
#include "ui_dialogforcemultipleregisters.h"

///
/// \brief DialogForceMultipleRegisters::DialogForceMultipleRegisters
/// \param params
/// \param length
/// \param parent
///
DialogForceMultipleRegisters::DialogForceMultipleRegisters(ModbusWriteParams& params, int length, QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::DialogForceMultipleRegisters)
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
/// \brief DialogForceMultipleRegisters::~DialogForceMultipleRegisters
///
DialogForceMultipleRegisters::~DialogForceMultipleRegisters()
{
    delete ui;
}

///
/// \brief DialogForceMultipleRegisters::accept
///
void DialogForceMultipleRegisters::accept()
{
    //_writeParams.Value = QVariant::fromValue(_data);
    QDialog::accept();
}

///
/// \brief DialogForceMultipleRegisters::on_pushButton0_clicked
///
void DialogForceMultipleRegisters::on_pushButton0_clicked()
{
    for(auto& v : _data)
    {
        v = 0;
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonRandom_clicked
///
void DialogForceMultipleRegisters::on_pushButtonRandom_clicked()
{
    for(auto& v : _data)
    {
        v = QRandomGenerator::global()->bounded(0, 65535);
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleRegisters::updateTableWidget
///
void DialogForceMultipleRegisters::updateTableWidget()
{
    const int columns = 8;
    const int fieldWidth = 4;
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
        const auto address1Text = QString("%1").arg(_writeParams.Address + i * columns, fieldWidth, 10, QLatin1Char('0'));
        const auto address2Text = QString("%1").arg(_writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), fieldWidth, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(address1Text, address2Text)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            auto numEdit = new NumericLineEdit(ui->tableWidget);

            QString text;
            switch(_writeParams.DisplayMode)
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Hex:
                    numEdit->setInputRange(0, 65535);
                    numEdit->setInputMode(NumericLineEdit::HexMode);
                    text = (idx < length) ? QString("%1H").arg(_data[idx], 4, 16, QLatin1Char('0')).toUpper() : "-";
                break;

                case DataDisplayMode::Decimal:
                case DataDisplayMode::Integer:
                    numEdit->setInputMode(NumericLineEdit::IntMode);
                    text = (idx < length) ?  QString::number(_data[idx]) : "-";
                break;

                case DataDisplayMode::FloatingPt:
                case DataDisplayMode::SwappedFP:
                break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
                break;
            }

            /*auto item = new QTableWidgetItem(text);

            item->setTextAlignment(Qt::AlignCenter);
            if(idx < length)
            {
                item->setData(Qt::UserRole, idx);
                item->setToolTip(QString("%1").arg(_writeParams.Address + idx, fieldWidth, 10, QLatin1Char('0')));
            }

            ui->tableWidget->setItem(i, j, item);*/

            numEdit->setFrame(false);
            numEdit->setAlignment(Qt::AlignCenter);
            numEdit->setMaximumWidth(60);
            numEdit->setValue(idx < length ? _data[idx] : 0);

            ui->tableWidget->setCellWidget(i, j, numEdit);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}
