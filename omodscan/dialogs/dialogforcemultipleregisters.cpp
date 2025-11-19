#include <QtMath>
#include <QRandomGenerator>
#include "formatutils.h"
#include "numericutils.h"
#include "numericlineedit.h"
#include "dialogforcemultipleregisters.h"
#include "ui_dialogforcemultipleregisters.h"

///
/// \brief DialogForceMultipleRegisters::DialogForceMultipleRegisters
/// \param params
/// \param length
/// \param hexAddress
/// \param parent
///
DialogForceMultipleRegisters::DialogForceMultipleRegisters(ModbusWriteParams& params, int length, bool hexAddress, QWidget *parent)
    :  QFixedSizeDialog(parent)
    , ui(new Ui::DialogForceMultipleRegisters)
    ,_writeParams(params)
    ,_hexAddress(hexAddress)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);

    ui->labelAddress->setText(tr("Address: <b>%1</b>").arg(formatAddress(QModbusDataUnit::HoldingRegisters, params.Address, params.AddrSpace, _hexAddress)));
    ui->labelLength->setText(tr("Length: <b>%1</b>").arg(length));
    ui->labelSlaveDevice->setText(tr("Device Id: <b>%1</b>").arg(params.DeviceId, 3, 10, QLatin1Char('0')));

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Hex:       
            ui->lineEditValue->setLeadingZeroes(true);
            ui->lineEditValue->setInputMode(NumericLineEdit::HexMode);
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
        break;

        case DataDisplayMode::Ansi:
            ui->lineEditValue->setInputMode(NumericLineEdit::AnsiMode);
            ui->lineEditValue->setCodepage(params.Codepage);
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
        break;

        case DataDisplayMode::Int16:
            ui->lineEditValue->setInputMode(NumericLineEdit::Int32Mode);
            ui->lineEditValue->setInputRange(SHRT_MIN, SHRT_MAX);
        break;

        case DataDisplayMode::Int32:
        case DataDisplayMode::SwappedInt32:
            ui->lineEditValue->setInputMode(NumericLineEdit::Int32Mode);
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::UInt16:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputMode(NumericLineEdit::UInt32Mode);
            ui->lineEditValue->setInputRange(0, USHRT_MAX);
        break;

        case DataDisplayMode::UInt32:
        case DataDisplayMode::SwappedUInt32:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputMode(NumericLineEdit::UInt32Mode);
        break;

        case DataDisplayMode::Int64:
        case DataDisplayMode::SwappedInt64:
            ui->lineEditValue->setInputMode(NumericLineEdit::Int64Mode);
        break;

        case DataDisplayMode::UInt64:
        case DataDisplayMode::SwappedUInt64:
            ui->lineEditValue->setLeadingZeroes(params.LeadingZeros);
            ui->lineEditValue->setInputMode(NumericLineEdit::UInt64Mode);
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            ui->lineEditValue->setInputMode(NumericLineEdit::FloatMode);
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            ui->lineEditValue->setInputMode(NumericLineEdit::DoubleMode);
        break;
    }

    _data = params.Value.value<QVector<quint16>>();
    if(_data.length() != length) _data.resize(length);

    updateTableWidget();
    adjustSize();
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
                case DataDisplayMode::Ansi:
                case DataDisplayMode::UInt16:
                case DataDisplayMode::Int16:
                {
                    auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                    _data[idx] = toByteOrderValue(numEdit->value<quint16>(), _writeParams.Order);
                }
                break;

                case DataDisplayMode::Int32:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakInt32(numEdit->value<qint32>(), _data[idx], _data[idx + 1], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedInt32:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakInt32(numEdit->value<qint32>(), _data[idx + 1], _data[idx], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::UInt32:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakUInt32(numEdit->value<quint32>(), _data[idx], _data[idx + 1], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedUInt32:
                    if(!(idx % 2) && (idx + 1 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakUInt32(numEdit->value<quint32>(), _data[idx + 1], _data[idx], _writeParams.Order);
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

                case DataDisplayMode::Int64:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakInt64(numEdit->value<qint64>(), _data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedInt64:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakInt64(numEdit->value<qint64>(), _data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::UInt64:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakUInt64(numEdit->value<quint64>(), _data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order);
                    }
                break;

                case DataDisplayMode::SwappedUInt64:
                    if(!(idx % 4) && (idx + 3 < _data.size()))
                    {
                        auto numEdit = (NumericLineEdit*)ui->tableWidget->cellWidget(i, j);
                        breakUInt64(numEdit->value<quint64>(), _data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order);
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
            case DataDisplayMode::Ansi:
            case DataDisplayMode::UInt16:
                _data[i] = QRandomGenerator::global()->bounded(0, USHRT_MAX);
            break;

            case DataDisplayMode::Int16:
                _data[i] = QRandomGenerator::global()->bounded(SHRT_MIN, SHRT_MAX);
            break;

            case DataDisplayMode::Int32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakInt32(QRandomGenerator::global()->bounded(INT_MIN, INT_MAX), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakInt32(QRandomGenerator::global()->bounded(INT_MIN, INT_MAX), _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::UInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakUInt32(QRandomGenerator::global()->bounded(0U, UINT_MAX), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedUInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakUInt32(QRandomGenerator::global()->bounded(0U, UINT_MAX), _data[i + 1], _data[i], _writeParams.Order);
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

            case DataDisplayMode::Int64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakInt64((qint64)QRandomGenerator::global()->generate64(), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakInt64((qint64)QRandomGenerator::global()->generate64(), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::UInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakUInt64(QRandomGenerator::global()->generate64(), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedUInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakUInt64(QRandomGenerator::global()->generate64(), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
            break;
        }
    }

    updateTableWidget();
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonValue_clicked
///
void DialogForceMultipleRegisters::on_pushButtonValue_clicked()
{
    for(int i = 0; i < _data.size(); i++)
    {
        switch(_writeParams.DisplayMode)
        {
            case DataDisplayMode::Hex:
            case DataDisplayMode::Ansi:
            case DataDisplayMode::Binary:
            case DataDisplayMode::UInt16:
                _data[i] = ui->lineEditValue->value<quint16>();
            break;

            case DataDisplayMode::Int16:
                _data[i] = ui->lineEditValue->value<qint16>();
            break;

            case DataDisplayMode::Int32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakInt32(ui->lineEditValue->value<qint32>(), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakInt32(ui->lineEditValue->value<qint32>(), _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::UInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakUInt32(ui->lineEditValue->value<quint32>(), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedUInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakUInt32(ui->lineEditValue->value<quint32>(), _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::Int64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakInt64(ui->lineEditValue->value<qint64>(), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakInt64(ui->lineEditValue->value<qint64>(), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::UInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakUInt64(ui->lineEditValue->value<quint64>(), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedUInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakUInt64(ui->lineEditValue->value<quint64>(), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::FloatingPt:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakFloat(ui->lineEditValue->value<float>(), _data[i], _data[i + 1], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedFP:
                if(!(i % 2) && (i + 1 < _data.size()))
                    breakFloat(ui->lineEditValue->value<float>(), _data[i + 1], _data[i], _writeParams.Order);
            break;

            case DataDisplayMode::DblFloat:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakDouble(ui->lineEditValue->value<double>(), _data[i], _data[i + 1], _data[i + 2], _data[i + 3], _writeParams.Order);
            break;

            case DataDisplayMode::SwappedDbl:
                if(!(i % 4) && (i + 3 < _data.size()))
                    breakDouble(ui->lineEditValue->value<double>(), _data[i + 3], _data[i + 2], _data[i + 1], _data[i], _writeParams.Order);
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
            numEdit->setLeadingZeroes(true);
            numEdit->setValue(toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::Ansi:
            numEdit = new NumericLineEdit(NumericLineEdit::AnsiMode, ui->tableWidget);
            numEdit->setInputRange(0, USHRT_MAX);
            numEdit->setCodepage(_writeParams.Codepage);
            numEdit->setValue(toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::UInt16:
            numEdit = new NumericLineEdit(NumericLineEdit::Int32Mode, ui->tableWidget);
            numEdit->setInputRange(0, USHRT_MAX);
            numEdit->setLeadingZeroes(_writeParams.LeadingZeros);
            numEdit->setValue(toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::Int16:
            numEdit = new NumericLineEdit(NumericLineEdit::Int32Mode, ui->tableWidget);
            numEdit->setInputRange(SHRT_MIN, SHRT_MAX);
            numEdit->setValue((qint16)toByteOrderValue(_data[idx], _writeParams.Order));
        break;

        case DataDisplayMode::Int32:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::Int32Mode, ui->tableWidget);
                numEdit->setValue(makeInt32(_data[idx], _data[idx + 1], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedInt32:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::Int32Mode, ui->tableWidget);
                numEdit->setValue(makeInt32(_data[idx + 1], _data[idx], _writeParams.Order));
            }
        break;

        case DataDisplayMode::UInt32:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::UInt32Mode, ui->tableWidget);
                numEdit->setLeadingZeroes(_writeParams.LeadingZeros);
                numEdit->setValue(makeUInt32(_data[idx], _data[idx + 1], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedUInt32:
            if(!(idx % 2) && (idx + 1 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::UInt32Mode, ui->tableWidget);
                numEdit->setLeadingZeroes(_writeParams.LeadingZeros);
                numEdit->setValue(makeUInt32(_data[idx + 1], _data[idx], _writeParams.Order));
            }
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

        case DataDisplayMode::Int64:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::Int64Mode, ui->tableWidget);
                numEdit->setValue(makeInt64(_data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedInt64:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::Int64Mode, ui->tableWidget);
                numEdit->setValue(makeInt64(_data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order));
            }
        break;

        case DataDisplayMode::UInt64:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::UInt64Mode, ui->tableWidget);
                numEdit->setLeadingZeroes(_writeParams.LeadingZeros);
                numEdit->setValue(makeUInt64(_data[idx], _data[idx + 1], _data[idx + 2], _data[idx + 3], _writeParams.Order));
            }
        break;

        case DataDisplayMode::SwappedUInt64:
            if(!(idx % 4) && (idx + 3 < _data.size()))
            {
                numEdit = new NumericLineEdit(NumericLineEdit::UInt64Mode, ui->tableWidget);
                numEdit->setLeadingZeroes(_writeParams.LeadingZeros);
                numEdit->setValue(makeUInt64(_data[idx + 3], _data[idx + 2], _data[idx + 1], _data[idx], _writeParams.Order));
            }
        break;
    }

    if(numEdit)
    {
        numEdit->setFrame(false);
        numEdit->setFixedWidth(150);
        numEdit->setAlignment(Qt::AlignCenter);
        numEdit->setToolTip(formatAddress(QModbusDataUnit::HoldingRegisters, _writeParams.Address + idx, _writeParams.AddrSpace, _hexAddress));
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
    lineEdit->setFixedWidth(150);
    lineEdit->setEnabled(false);
    lineEdit->setAlignment(Qt::AlignCenter);
    return lineEdit;
}

///
/// \brief DialogForceMultipleRegisters::updateTableWidget
///
void DialogForceMultipleRegisters::updateTableWidget()
{
    const int columns = 5;
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
        const auto addressFrom = formatAddress(QModbusDataUnit::HoldingRegisters, _writeParams.Address + i * columns, _writeParams.AddrSpace, _hexAddress);
        const auto addressTo = formatAddress(QModbusDataUnit::HoldingRegisters, _writeParams.Address + qMin(length - 1, (i + 1) * columns - 1), _writeParams.AddrSpace, _hexAddress);
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
