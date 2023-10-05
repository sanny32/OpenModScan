#include <QKeyEvent>
#include <QLineEdit>
#include "modbusfunction.h"
#include "qhexvalidator.h"
#include "quintvalidator.h"
#include "functioncodecombobox.h"

///
/// \brief FunctionCodeComboBox::FunctionCodeComboBox
/// \param parent
///
FunctionCodeComboBox::FunctionCodeComboBox(QWidget *parent)
    : QComboBox(parent)
    ,_currentFunc(QModbusPdu::Invalid)
    ,_validator(nullptr)
{
    setInputMode(InputMode::DecMode);

    connect(this, &QComboBox::currentTextChanged, this, &FunctionCodeComboBox::on_currentTextChanged);
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FunctionCodeComboBox::on_currentIndexChanged);
}

///
/// \brief FunctionCodeComboBox::currentFunctionCode
/// \return
///
QModbusPdu::FunctionCode FunctionCodeComboBox::currentFunctionCode() const
{
    return _currentFunc;
}

///
/// \brief FunctionCodeComboBox::setCurrentFunctionCode
/// \param pointType
///
void FunctionCodeComboBox::setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode)
{
    _currentFunc = funcCode;

    const auto idx = findData(funcCode);
    if(idx != -1) setCurrentIndex(idx);
    else setCurrentText(formatFuncCode(funcCode));
}

///
/// \brief FunctionCodeComboBox::inputMode
/// \return
///
FunctionCodeComboBox::InputMode FunctionCodeComboBox::inputMode() const
{
    return _inputMode;
}

///
/// \brief FunctionCodeComboBox::setInputMode
/// \param on
///
void FunctionCodeComboBox::setInputMode(FunctionCodeComboBox::InputMode mode)
{
    if(!isEditable())
        return;

    _inputMode = mode;

    if(_validator)
    {
        delete _validator;
        _validator = nullptr;
    }

    switch(mode)
    {
        case DecMode:
            _validator = new QUIntValidator(0, 255, this);
        break;

        case HexMode:
           _validator = new QHexValidator(0, 0xFF, this);
        break;
    }

    blockSignals(true);
    lineEdit()->blockSignals(true);
    for (int i = 0; i < count(); i++)
    {
        const auto funcCode = itemData(i).value<QModbusPdu::FunctionCode>();
        setItemText(i, QString("%1: %2").arg(formatFuncCode(funcCode), ModbusFunction(funcCode)));
    }
    blockSignals(false);
    lineEdit()->blockSignals(false);

    update();
}

///
/// \brief FunctionCodeComboBox::addItem
/// \param funcCode
///
void FunctionCodeComboBox::addItem(QModbusPdu::FunctionCode funcCode)
{
    const auto code = formatFuncCode(funcCode);
    QComboBox::addItem(QString("%1: %2").arg(code, ModbusFunction(funcCode)), funcCode);
}

///
/// \brief FunctionCodeComboBox::addAllItems
///
void FunctionCodeComboBox::addAllItems()
{
    static const auto items = {
        QModbusPdu::ReadCoils, QModbusPdu::ReadDiscreteInputs, QModbusPdu::ReadHoldingRegisters, QModbusPdu::ReadInputRegisters,
        QModbusPdu::WriteSingleCoil, QModbusPdu::WriteSingleRegister, QModbusPdu::ReadExceptionStatus, QModbusPdu::Diagnostics,
        QModbusPdu::GetCommEventCounter, QModbusPdu::GetCommEventLog, QModbusPdu::WriteMultipleCoils, QModbusPdu::WriteMultipleRegisters,
        QModbusPdu::ReportServerId, QModbusPdu::ReadFileRecord, QModbusPdu::WriteFileRecord, QModbusPdu::MaskWriteRegister,
        QModbusPdu::ReadWriteMultipleRegisters, QModbusPdu::ReadFifoQueue, QModbusPdu::EncapsulatedInterfaceTransport
    };

    for(auto&& item : items)
        addItem(item);
}

///
/// \brief FunctionCodeComboBox::update
///
void FunctionCodeComboBox::update()
{
    const auto idx = findData(_currentFunc, Qt::UserRole);
    if(idx != -1)
        setCurrentIndex(idx);
    else
        setCurrentText(formatFuncCode(_currentFunc));
}

///
/// \brief FunctionCodeComboBox::formatFuncCode
/// \param funcCode
/// \return
///
QString FunctionCodeComboBox::formatFuncCode(QModbusPdu::FunctionCode funcCode) const
{
    switch(_inputMode)
    {
        case DecMode:
            return QString("%1").arg(QString::number(funcCode), 2, '0');

        case HexMode:
            return (isEditable() && hasFocus() ? "" : "0x") + QString("%1").arg(QString::number(funcCode, 16).toUpper(), 2, '0');

        default:
            return QString::number(funcCode);
    }
}

///
/// \brief FunctionCodeComboBox::on_currentIndexChanged
/// \param index
///
void FunctionCodeComboBox::on_currentIndexChanged(int index)
{
    if(index >= 0)
    {
        _currentFunc = itemData(index).value<QModbusPdu::FunctionCode>();
        emit functionCodeChanged(_currentFunc);
    }
}

///
/// \brief FunctionCodeComboBox::on_currentTextChanged
///
void FunctionCodeComboBox::on_currentTextChanged(const QString& text)
{
    const auto idx = findData(text, Qt::DisplayRole);
    if(idx != -1)
    {
        _currentFunc = itemData(idx).value<QModbusPdu::FunctionCode>();
    }
    else
    {
        bool ok;
        quint8 func;

        switch(_inputMode)
        {
            case InputMode::DecMode:
                func = text.toUInt(&ok);
            break;

            case InputMode::HexMode:
                func = text.toUInt(&ok, 16);
            break;
        }

        _currentFunc = ok ? (QModbusPdu::FunctionCode)func : QModbusPdu::Invalid;
    }
}

///
/// \brief FunctionCodeComboBox::focusInEvent
/// \param e
///
void FunctionCodeComboBox::focusInEvent(QFocusEvent* e)
{
    if(isEditable())
        setCurrentText(formatFuncCode(_currentFunc));

    QComboBox::focusInEvent(e);
}

///
/// \brief FunctionCodeComboBox::focusOutEvent
/// \param e
///
void FunctionCodeComboBox::focusOutEvent(QFocusEvent* e)
{
    update();
    QComboBox::focusOutEvent(e);
}

///
/// \brief FunctionCodeComboBox::keyPressEvent
/// \param e
///
void FunctionCodeComboBox::keyPressEvent(QKeyEvent* e)
{
    if(!_validator || !isEditable())
    {
        QComboBox::keyPressEvent(e);
        return;
    }

    int pos = 0;
    auto text = e->text();
    const auto state = _validator->validate(text, pos);

    if(state == QValidator::Acceptable ||
        e->key() == Qt::Key_Backspace ||
        e->key() == Qt::Key_Delete ||
        e->matches(QKeySequence::Cut) ||
        e->matches(QKeySequence::Copy) ||
        e->matches(QKeySequence::Paste) ||
        e->matches(QKeySequence::Undo) ||
        e->matches(QKeySequence::Redo) ||
        e->matches(QKeySequence::SelectAll))
    {
        QComboBox::keyPressEvent(e);
    }
}
