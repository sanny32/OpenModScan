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
{
    ui->setupUi(this);
    ui->lineEditAddress->setInputRange(1, 65534);
    ui->lineEditAddress->enablePaddingZero(true);
    ui->lineEditLength->setInputRange(1, 128);
    ui->lineEditDeviceId->setInputRange(1, 255);

    setWindowTitle(QString("ModSca%1").arg(num));
    ui->outputWidget->update(displayDefinition());

    connect(&_timer, &QTimer::timeout, this, &FormModSca::on_timeout);
    _timer.setInterval(1000);
    _timer.start();
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
    dd.ScanRate = _timer.interval();
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
    _timer.setInterval(dd.ScanRate);
    ui->lineEditDeviceId->setValue(dd.DeviceId);
    ui->lineEditAddress->setValue(dd.PointAddress);
    ui->lineEditLength->setValue(dd.Length);
    ui->comboBoxModbusPointType->setCurrentPointType(dd.PointType);

    ui->outputWidget->update(dd);
}

///
/// \brief FormModSca::displayMode
/// \return
///
DisplayMode FormModSca::displayMode() const
{
    return ui->outputWidget->displayMode();
}

///
/// \brief FormModSca::setDisplayMode
/// \param mode
///
void FormModSca::setDisplayMode(DisplayMode mode)
{
    ui->outputWidget->setDisplayMode(mode);
}

///
/// \brief FormModSca::dataDisplayMode
/// \return
///
DataDisplayMode FormModSca::dataDisplayMode() const
{
    return ui->outputWidget->dataDisplayMode();
}

///
/// \brief FormModSca::setDataDisplayMode
/// \param mode
///
void FormModSca::setDataDisplayMode(DataDisplayMode mode)
{
    ui->outputWidget->setDataDisplayMode(mode);
}

///
/// \brief FormModSca::on_timeout
///
void FormModSca::on_timeout()
{

}

///
/// \brief FormModSca::on_comboBoxModbusPointType_currentTextChanged
///
void FormModSca::on_comboBoxModbusPointType_currentTextChanged(const QString&)
{
    ui->outputWidget->update(displayDefinition());
}
