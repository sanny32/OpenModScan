#include <QtMath>
#include <QRandomGenerator>
#include "floatutils.h"
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

    ui->labelAddress->setText(QString(tr("Address: %1")).arg(params.Address, 5, 10, QLatin1Char('0')));
    ui->labelLength->setText(QString(tr("Length: %1")).arg(length, 3, 10, QLatin1Char('0')));

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
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            const auto idx = i *  ui->tableWidget->columnCount() + j;
            if(idx >= _data.size())
            {
                break;
            }

            switch(_writeParams.DisplayMode)
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Hex:
                case DataDisplayMode::Decimal:
                case DataDisplayMode::Integer:
                {
                    auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                    _data[idx] = toByteOrderValue(numEdit->value<quint16>(), _writeParams.Order);
                }
                break;

                case DataDisplayMode::FloatingPt:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakFloat(numEdit->value<double>(), _data[idx], _data[idx + 1], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedFP:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakFloat(numEdit->value<double>(), _data[idx + 1], _data[idx], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::DblFloat:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakDouble(numEdit->value<double>(), _data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedDbl:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakDouble(numEdit->value<double>(), _data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order);
                    }
                break;
            }
        }
    }

    _writeParams.Value = QVariant::fromValue(_data);
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
    for(int i = 0; i < _data.size(); i++)
    {
        switch(_writeParams.DisplayMode)
        {
            case DataDisplayMode::Binary:
            case DataDisplayMode::Hex:
            case DataDisplayMode::Decimal:
                _data[i] = QRandomGenerator::global()->bounded(0, USHRT_MAX);
            break;

            case DataDisplayMode::Integer:
                _data[i] = QRandomGenerator::global()->bounded(SHRT_MIN, SHRT_MAX);
            break;

            case DataDisplayMode::FloatingPt:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakFloat(QRandomGenerator::global()->bounded(100.), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedFP:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakFloat(QRandomGenerator::global()->bounded(100.), _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::DblFloat:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakDouble(QRandomGenerator::global()->bounded(100.), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedDbl:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakDouble(QRandomGenerator::global()->bounded(100.), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
            break;
        }
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleRegisters::createNumEdit
/// \param idx
/// \return
///
NumericLineEdit* DialogForceMultipleRegisters::createNumEdit(int idx)
{
    NumericLineEdit* numEdit = nullptr;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Binary:
        case DataDisplayMode::Hex:
            numEdit = new NumericLineEdit(NumericLineEdit::HexMode, ui->tableWidget);
            numEdit->setInputRange(0, USHRT_MAX);
            numEdit->setPaddingZeroes(true);
            numEdit->setValue(toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::Decimal:
            numEdit = new NumericLineEdit(NumericLineEdit::DecMode, ui->tableWidget);
            numEdit->setInputRange(0, USHRT_MAX);
            numEdit->setValue(toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::Integer:
            numEdit = new NumericLineEdit(NumericLineEdit::DecMode, ui->tableWidget);
            numEdit->setInputRange(SHRT_MIN, SHRT_MAX);
            numEdit->setValue((qint16)toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::FloatingPt:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::FloatMode, ui->tableWidget);
                numEdit->setValue(makeFloat(_data[idx], _data[idx + 1], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedFP:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::FloatMode, ui->tableWidget);
                numEdit->setValue(makeFloat(_data[idx + 1], _data[idx], _writeParams.Order));
            }
        break;

        case DataDisplayMode::DblFloat:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::DoubleMode, ui->tableWidget);
                numEdit->setValue(makeDouble(_data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedDbl:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::DoubleMode, ui->tableWidget);
                numEdit->setValue(makeDouble(_data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order));
            }
        break;
    }

    if(numEdit)
    {
        numEdit->setFrame(false);
        numEdit->setMaximumWidth(80);
        numEdit->setAlignment(Qt::AlignCenter);
        numEdit->setToolTip(QString("%1").arg(_writeParams.Address + idx, 5, 10, QLatin1Char('0')));
    }

    return numEdit;
}

///
/// \brief DialogForceMultipleRegisters::createLineEdit
/// \return
///
QLineEdit* DialogForceMultipleRegisters::createLineEdit()
{
    auto lineEdit = new QLineEdit(ui->tableWidget);
    lineEdit->setText("-");
    lineEdit->setFrame(false);
    lineEdit->setMaximumWidth(80);
    lineEdit->setEnabled(false);
    lineEdit->setAlignment(Qt::AlignCenter);
    return lineEdit;
}

///
/// \brief DialogForceMultipleRegisters::updateTableWidget
///
void DialogForceMultipleRegisters::updateTableWidget()
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
        const auto addressFrom = QString("%1").arg(_writeParams.Address + i * columns, 5, 10, QLatin1Char('0'));
        const auto addressTo = QString("%1").arg(_writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), 5, 10, QLatin1Char('0'));
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1-%2").arg(addressFrom, addressTo)));

        for(int j = 0; j < columns; j++)
        {
            const auto idx = i * columns + j;
            if(idx < length)
            {
                auto numEdit = createNumEdit(idx);
                if(numEdit) ui->tableWidget->setCellWidget(i, j, numEdit);
                else ui->tableWidget->setCellWidget(i, j, createLineEdit());
            }
            else
            {
                ui->tableWidget->setCellWidget(i, j, createLineEdit());
            }
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}
