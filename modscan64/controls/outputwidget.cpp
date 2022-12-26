#include "formmodsca.h"
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
/// \brief OutputWidget::update
///
void OutputWidget::update(const DisplayDefinition& dd)
{
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
    for(int i = 0; i < _displayDefinition.Length; i++)
    {
        const auto addr = QStringLiteral("%1").arg(i + _displayDefinition.PointAddress, 4, 10, QLatin1Char('0'));

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

///
/// \brief OutputWidget::updateTrafficWidget
///
void OutputWidget::updateTrafficWidget()
{

}
