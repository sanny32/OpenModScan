#include <QtMath>
#include <QPainter>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFileDialog>
#include "uiutils.h"
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
    : QAdjustedSizeDialog(parent)
    , ui(new Ui::DialogForceMultipleRegisters)
    ,_writeParams(params)
    ,_hexAddress(hexAddress)
{
    ui->setupUi(this);

    ui->labelAddress->setText(QString(ui->labelAddress->text()).arg(formatAddress(QModbusDataUnit::HoldingRegisters, params.Address, params.AddrSpace, _hexAddress)));
    ui->labelLength->setText(QString( ui->labelLength->text()).arg(length));
    ui->labelSlaveDevice->setText(QString(ui->labelSlaveDevice->text()).arg(params.DeviceId, 3, 10, QLatin1Char('0')));
    ui->labelAddresses->setText(QString(ui->labelAddresses->text()).arg(
        formatAddress(QModbusDataUnit::HoldingRegisters, params.Address, params.AddrSpace, _hexAddress),
        formatAddress(QModbusDataUnit::HoldingRegisters, params.Address + length - 1, params.AddrSpace, _hexAddress)));

    recolorPushButtonIcon(ui->pushButtonExport, Qt::red);
    recolorPushButtonIcon(ui->pushButtonImport, Qt::darkGreen);
    recolorPushButtonIcon(ui->pushButtonValue, Qt::darkMagenta);

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
    if(_data.length() != length) {
        _data.resize(length);
    }

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
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<qint32>(qint32 value, int index, ValueOperation op)
{
    qint32 cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Int32:        cur = makeInt32(_data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedInt32: cur = makeInt32(_data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Int32:        breakInt32(cur, _data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedInt32: breakInt32(cur, _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<quint32>(quint32 value, int index, ValueOperation op)
{
    quint32 cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::UInt32:        cur = makeUInt32(_data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedUInt32: cur = makeUInt32(_data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::UInt32:        breakUInt32(cur, _data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedUInt32: breakUInt32(cur, _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<float>(float value, int index, ValueOperation op)
{
    float cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::FloatingPt: cur = makeFloat(_data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedFP:  cur = makeFloat(_data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::FloatingPt: breakFloat(cur, _data[index], _data[index + 1], _writeParams.Order); break;
        case DataDisplayMode::SwappedFP:  breakFloat(cur, _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<qint64>(qint64 value, int index, ValueOperation op)
{
    qint64 cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Int64:        cur = makeInt64(_data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedInt64: cur = makeInt64(_data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::Int64:        breakInt64(cur, _data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedInt64: breakInt64(cur, _data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<quint64>(quint64 value, int index, ValueOperation op)
{
    quint64 cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::UInt64:        cur = makeUInt64(_data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedUInt64: cur = makeUInt64(_data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::UInt64:        breakUInt64(cur, _data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedUInt64: breakUInt64(cur, _data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyValue
/// \param value
/// \param index
/// \param op
///
template<>
void DialogForceMultipleRegisters::applyValue<double>(double value, int index, ValueOperation op)
{
    double cur;
    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::DblFloat:   cur = makeDouble(_data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedDbl: cur = makeDouble(_data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: return;
    }

    switch(op)
    {
        case ValueOperation::Set:       cur = value; break;
        case ValueOperation::Add:       cur += value; break;
        case ValueOperation::Subtract:  cur -= value; break;
        case ValueOperation::Multiply:  cur *= value; break;
        case ValueOperation::Divide:    cur /= value; break;
    }

    switch(_writeParams.DisplayMode)
    {
        case DataDisplayMode::DblFloat:   breakDouble(cur, _data[index], _data[index + 1], _data[index + 2], _data[index + 3], _writeParams.Order); break;
        case DataDisplayMode::SwappedDbl: breakDouble(cur, _data[index + 3], _data[index + 1], _data[index + 1], _data[index], _writeParams.Order); break;
        default: break;
    }
}

///
/// \brief DialogForceMultipleRegisters::applyToAll
/// \param op
/// \param value
///
void DialogForceMultipleRegisters::applyToAll(ValueOperation op, double value)
{
    for(int i = 0; i < _data.size(); i++)
    {
        switch(_writeParams.DisplayMode)
        {
            case DataDisplayMode::Hex:
            case DataDisplayMode::Ansi:
            case DataDisplayMode::Binary:
            case DataDisplayMode::UInt16:
                applyValue<quint16>(static_cast<quint16>(value), i, op);
            break;

            case DataDisplayMode::Int16:
                applyValue<qint16>(static_cast<qint16>(value), i, op);
            break;

            case DataDisplayMode::Int32:
            case DataDisplayMode::SwappedInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    applyValue<qint32>(static_cast<qint32>(value), i, op);
            break;

            case DataDisplayMode::UInt32:
            case DataDisplayMode::SwappedUInt32:
                if(!(i % 2) && (i + 1 < _data.size()))
                    applyValue<quint32>(static_cast<quint32>(value), i, op);
            break;

            case DataDisplayMode::Int64:
            case DataDisplayMode::SwappedInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    applyValue<qint64>(static_cast<qint64>(value), i, op);
            break;

            case DataDisplayMode::UInt64:
            case DataDisplayMode::SwappedUInt64:
                if(!(i % 4) && (i + 3 < _data.size()))
                    applyValue<quint64>(static_cast<quint64>(value), i, op);
            break;

            case DataDisplayMode::FloatingPt:
            case DataDisplayMode::SwappedFP:
                if(!(i % 2) && (i + 1 < _data.size()))
                    applyValue<float>(static_cast<float>(value), i, op);
            break;

            case DataDisplayMode::DblFloat:
            case DataDisplayMode::SwappedDbl:
                if(!(i % 4) && (i + 3 < _data.size()))
                    applyValue<double>(static_cast<double>(value), i, op);
            break;
        }
    }

    updateTableWidget();
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
    applyToAll(ValueOperation::Set, 0);
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
                applyValue<quint16>(QRandomGenerator::global()->bounded(0, USHRT_MAX), i, ValueOperation::Set);
            break;

            case DataDisplayMode::Int16:
                applyValue<qint16>(QRandomGenerator::global()->bounded(SHRT_MIN, USHRT_MAX), i, ValueOperation::Set);
            break;

            case DataDisplayMode::Int32:
            case DataDisplayMode::SwappedInt32:
                applyValue<qint32>(QRandomGenerator::global()->bounded(INT_MIN, INT_MAX), i, ValueOperation::Set);
            break;

            case DataDisplayMode::UInt32:
            case DataDisplayMode::SwappedUInt32:
                applyValue<quint32>(QRandomGenerator::global()->bounded(0U, UINT_MAX), i, ValueOperation::Set);
            break;

            case DataDisplayMode::FloatingPt:
            case DataDisplayMode::SwappedFP:
                applyValue<float>(QRandomGenerator::global()->bounded(100), i, ValueOperation::Set);
            break;

            case DataDisplayMode::DblFloat:
            case DataDisplayMode::SwappedDbl:
                applyValue<double>(QRandomGenerator::global()->bounded(100), i, ValueOperation::Set);
            break;


            case DataDisplayMode::Int64:
            case DataDisplayMode::SwappedInt64:
                applyValue<qint64>(QRandomGenerator::global()->generate64(), i, ValueOperation::Set);
            break;

            case DataDisplayMode::UInt64:
            case DataDisplayMode::SwappedUInt64:
                applyValue<quint64>(QRandomGenerator::global()->generate64(), i, ValueOperation::Set);
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
    applyToAll(ValueOperation::Set, ui->lineEditValue->value<double>());
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonSub1_clicked
///
void DialogForceMultipleRegisters::on_pushButtonSub1_clicked()
{
    applyToAll(ValueOperation::Subtract, 1);
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonAdd1_clicked
///
void DialogForceMultipleRegisters::on_pushButtonAdd1_clicked()
{
    applyToAll(ValueOperation::Add, 1);
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonAdd2_clicked
///
void DialogForceMultipleRegisters::on_pushButtonAdd2_clicked()
{
    applyToAll(ValueOperation::Add, 2);
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonAdd3_clicked
///
void DialogForceMultipleRegisters::on_pushButtonAdd3_clicked()
{
    applyToAll(ValueOperation::Add, 3);
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonAdd4_clicked
///
void DialogForceMultipleRegisters::on_pushButtonAdd4_clicked()
{
    applyToAll(ValueOperation::Add, 4);
}

///
/// \brief DialogForceMultipleRegisters::on_pushButtonImport_clicked
///
void DialogForceMultipleRegisters::on_pushButtonImport_clicked()
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
        quint16 value = 0;

        if(valueStr.startsWith("0x", Qt::CaseInsensitive)) {
            value = valueStr.mid(2).toUShort(&ok, 16);
        }
        else {
            value = valueStr.toUShort(&ok, 10);
        }

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
/// \brief DialogForceMultipleRegisters::on_pushButtonExport_clicked
///
void DialogForceMultipleRegisters::on_pushButtonExport_clicked()
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

