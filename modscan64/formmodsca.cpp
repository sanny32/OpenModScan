#include <QtDebug>
#include "formmodsca.h"
#include "ui_formmodsca.h"

///
/// \brief FormModSca::FormModSca
/// \param num
/// \param parent
///
FormModSca::FormModSca(int num, QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::FormModSca)
    ,_displayMode(DisplayMode::Data)
    ,_dataDisplayMode(DataDisplayMode::Binary)
{
    ui->setupUi(this);
    ui->lineEditAddress->setInputRange(1, 65534);
    ui->lineEditAddress->enablePaddingZero(true);
    ui->lineEditLength->setInputRange(1, 128);
    ui->lineEditDeviceId->setInputRange(1, 255);

    setWindowTitle(QString("ModSca%1").arg(num));
    updateListWidget();
}

///
/// \brief FormModSca::~FormModSca
///
FormModSca::~FormModSca()
{
    delete ui;
}

///
/// \brief FormModSca::displayDefinition
/// \return
///
DisplayDefinition FormModSca::displayDefinition() const
{
    DisplayDefinition dd;
    dd.ScanRate = _scanRate;
    dd.DeviceId = ui->lineEditDeviceId->value();
    dd.PointAddress = ui->lineEditAddress->value();
    dd.PointType = ui->comboBoxModbusPointType->currentPointType();
    dd.Length = ui->lineEditLength->value();

    return dd;
}

///
/// \brief FormModSca::setDisplayDefinition
/// \param dd
///
void FormModSca::setDisplayDefinition(const DisplayDefinition& dd)
{
    _scanRate = dd.ScanRate;
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);

    updateListWidget();
}

///
/// \brief FormModSca::displayMode
/// \return
///
DisplayMode FormModSca::displayMode() const
{
    return _displayMode;
}

///
/// \brief FormModSca::setDisplayMode
/// \param mode
///
void FormModSca::setDisplayMode(DisplayMode mode)
{
    _displayMode = mode;
}

///
/// \brief FormModSca::dataDisplayMode
/// \return
///
DataDisplayMode FormModSca::dataDisplayMode() const
{
    return _dataDisplayMode;
}

///
/// \brief FormModSca::setDataDisplayMode
/// \param mode
///
void FormModSca::setDataDisplayMode(DataDisplayMode mode)
{
    _dataDisplayMode = mode;
    updateListWidget();
}

///
/// \brief FormModSca::on_comboBoxModbusPointType_currentTextChanged
///
void FormModSca::on_comboBoxModbusPointType_currentTextChanged(const QString&)
{
    updateListWidget();
}

///
/// \brief FormModSca::updateListWidget
///
void FormModSca::updateListWidget()
{
    QString prefix;
    const auto modbusPointType = ui->comboBoxModbusPointType->currentPointType();
    switch(modbusPointType)
    {
        case 1:
            prefix = "0";
        break;
        case 2:
            prefix = "1";
        break;
        case 3:
            prefix = "4";
        break;
        case 4:
            prefix = "3";
        break;
    }

    ui->listWidget->clear();
    const auto startAddr = ui->lineEditAddress->value();
    for(int i = 0; i < ui->lineEditLength->value(); i++)
    {
        const auto addr = QStringLiteral("%1").arg(i + startAddr, 4, 10, QLatin1Char('0'));

        short value = 0;
        QString valstr;
        switch(_dataDisplayMode)
        {
            case DataDisplayMode::Binary:
                valstr = QStringLiteral("%1").arg(value, 16, 2, QLatin1Char('0'));
            break;

            case DataDisplayMode::Decimal:
                valstr = QStringLiteral("%1").arg(value, 5, 10, QLatin1Char('0'));
            break;

            case DataDisplayMode::Integer:
                valstr = QStringLiteral("%1").arg(value, 5, 10, QLatin1Char(' '));
            break;

            case DataDisplayMode::Hex:
                valstr = QStringLiteral("%1H").arg(value, 4, 16, QLatin1Char('0'));
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

        const auto label = QString("%1%2: <%3>                ").arg(prefix, addr, valstr);
        ui->listWidget->addItem(label);
    }
}
