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
    const int addrFieldWidth = 4;
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
        const auto addressFrom = QString("%1").arg(_writeParams.Address + i * columns, addrFieldWidth, 10, QLatin1Char('0'));
        const auto addressTo = QString("%1").arg(_writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), addrFieldWidth, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(addressFrom, addressTo)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            if(idx < length)
            {
                auto numEdit = new NumericLineEdit(ui->tableWidget);
                numEdit->setFrame(false);
                numEdit->setMaximumWidth(60);
                numEdit->setAlignment(Qt::AlignCenter);
                numEdit->setToolTip(QString("%1").arg(_writeParams.Address + idx, addrFieldWidth, 10, QLatin1Char('0')));

                QString text;
                switch(_writeParams.DisplayMode)
                {
                    case DataDisplayMode::Binary:
                    case DataDisplayMode::Hex:
                        numEdit->setInputRange(0, 65535);
                        numEdit->setPaddingZeroes(true);
                        numEdit->setInputMode(NumericLineEdit::HexMode);
                    break;

                    case DataDisplayMode::Decimal:
                    case DataDisplayMode::Integer:
                        numEdit->setInputRange(0, 65535);
                        numEdit->setInputMode(NumericLineEdit::IntMode);
                    break;

                    case DataDisplayMode::FloatingPt:
                    case DataDisplayMode::SwappedFP:
                    break;

                    case DataDisplayMode::DblFloat:
                    case DataDisplayMode::SwappedDbl:
                    break;
                }

                numEdit->setValue(_data[idx]);
                ui->tableWidget->setCellWidget(i, j, numEdit);
            }
            else
            {
                auto lineEdit = new QLineEdit(ui->tableWidget);
                lineEdit->setText("-");
                lineEdit->setFrame(false);
                lineEdit->setMaximumWidth(60);
                lineEdit->setEnabled(false);
                lineEdit->setAlignment(Qt::AlignCenter);
                ui->tableWidget->setCellWidget(i, j, lineEdit);
            }
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}
