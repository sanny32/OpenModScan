#include <QDebug>
#include "outputwidget.h"
#include "ui_outputwidget.h"

///
/// \brief OutputWidget::OutputWidget
/// \param parent
///
OutputWidget::OutputWidget(QWidget *parent) :
     QWidget(parent)
   , ui(new Ui::OutputWidget)
   ,_displayMode(DisplayMode::Data)
   ,_dataDisplayMode(DataDisplayMode::Binary)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
}

///
/// \brief OutputWidget::~OutputWidget
///
OutputWidget::~OutputWidget()
{
    delete ui;
}

///
/// \brief OutputWidget::setStatus
/// \param status
///
void OutputWidget::setStatus(const QString& status)
{
    if(status.isEmpty())
        ui->labelStatus->setText(QString());
    else
        ui->labelStatus->setText(QString("** %1 **").arg(status));
}

///
/// \brief OutputWidget::update
///
void OutputWidget::update(const DisplayDefinition& dd, const QModbusDataUnit& data)
{
    _displayData = data;
    _displayDefinition = dd;
    switch(displayMode())
    {
        case DisplayMode::Data:
            updateDataWidget();
        break;

        case DisplayMode::Traffic:
            updateTrafficWidget();
        break;
    }
}

///
/// \brief OutputWidget::displayMode
/// \return
///
DisplayMode OutputWidget::displayMode() const
{
    return _displayMode;
}

///
/// \brief OutputWidget::setDisplayMode
/// \param mode
///
void OutputWidget::setDisplayMode(DisplayMode mode)
{
    _displayMode = mode;
    switch(mode)
    {
        case DisplayMode::Data:
            ui->stackedWidget->setCurrentIndex(0);
        break;

        case DisplayMode::Traffic:
            ui->stackedWidget->setCurrentIndex(1);
        break;
    }
}

///
/// \brief OutputWidget::dataDisplayMode
/// \return
///
DataDisplayMode OutputWidget::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief OutputWidget::setDataDisplayMode
/// \param mode
///
void OutputWidget::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    updateDataWidget();
}

///
/// \brief OutputWidget::updateDataWidget
///
void OutputWidget::updateDataWidget()
{
    QString prefix;
    switch(_displayDefinition.PointType)
    {
        case QModbusDataUnit::Coils:
            prefix = "0";
        break;
        case QModbusDataUnit::DiscreteInputs:
            prefix = "1";
        break;
        case QModbusDataUnit::HoldingRegisters:
            prefix = "4";
        break;
        case QModbusDataUnit::InputRegisters:
            prefix = "3";
        break;
        default:
        break;
    }

    ui->listWidget->clear();
    for(int i = 0; i < _displayDefinition.Length; i++)
    {
        const auto addr = QStringLiteral("%1").arg(i + _displayDefinition.PointAddress, 4, 10, QLatin1Char('0'));
        const auto value1 = _displayData.isValid() ? _displayData.value(i) : 0;
        const auto value2 = _displayData.isValid() ? _displayData.value(i + 1) : 0;
        const auto valstr = formatDataValue(_displayDefinition.PointType, _dataDisplayMode, value1, value2);
        const auto label = QString("%1%2: <%3>                ").arg(prefix, addr, valstr);
        ui->listWidget->addItem(label);
    }
}

///
/// \brief OutputWidget::updateTrafficWidget
///
void OutputWidget::updateTrafficWidget()
{

}

///
/// \brief formatBinatyValue
/// \param pointType
/// \param value
/// \return
///
QString formatBinatyValue(QModbusDataUnit::RegisterType pointType, quint16 value)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("%1").arg(value, 1, 2, QLatin1Char('0'));
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("%1").arg(value, 16, 2, QLatin1Char('0'));
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatDecimalValue
/// \param pointType
/// \param value
/// \return
///
QString formatDecimalValue(QModbusDataUnit::RegisterType pointType, quint16 value)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("%1").arg(value, 1, 10, QLatin1Char('0'));
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("%1").arg(value, 5, 10, QLatin1Char('0'));
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatIntegerValue
/// \param pointType
/// \param value
/// \return
///
QString formatIntegerValue(QModbusDataUnit::RegisterType pointType, qint16 value)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString::number(value);
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("%1").arg(value, 5, 10, QLatin1Char(' '));
        break;
        default:
        break;
    }
    return result;
}

///
/// \brief formatHexValue
/// \param pointType
/// \param value
/// \return
///
QString formatHexValue(QModbusDataUnit::RegisterType pointType, quint16 value)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("%1").arg(value, 1, 16, QLatin1Char('0'));
        break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("%1H").arg(value, 4, 16, QLatin1Char('0'));
        break;
        default:
        break;
    }
    return result.toUpper();
}

///
/// \brief OutputWidget::formatDataValue
/// \param pointType
/// \param mode
/// \param value1
/// \param value2
/// \return
///
QString OutputWidget::formatDataValue(QModbusDataUnit::RegisterType pointType, DataDisplayMode mode, quint16 value1, quint16 value2)
{
    QString result;
    switch(mode)
    {
        case DataDisplayMode::Binary:
            result = formatBinatyValue(pointType, value1);
        break;

        case DataDisplayMode::Decimal:
            result = formatDecimalValue(pointType, value1);
        break;

        case DataDisplayMode::Integer:
            result = formatIntegerValue(pointType, value1);
        break;

        case DataDisplayMode::Hex:
            result = formatHexValue(pointType, value1);
        break;

        case DataDisplayMode::FloatingPt:
        break;

        case DataDisplayMode::SwappedFP:
        break;

        case DataDisplayMode::DblFloat:
        break;

        case DataDisplayMode::SwappedDbl:
        break;
    }

    return result;
}
