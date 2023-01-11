#include <QPrinterInfo>
#include <QPushButton>
#include "dialogprintsettings.h"
#include "ui_dialogprintsettings.h"

Q_DECLARE_METATYPE(QPrinterInfo)

///
/// \brief DialogPrintSettings::DialogPrintSettings
/// \param parent
///
DialogPrintSettings::DialogPrintSettings(QPrinter* printer, QWidget *parent) :
     QFixedSizeDialog(parent)
    , ui(new Ui::DialogPrintSettings)
    ,_printer(printer)
{    
    ui->setupUi(this);

    const auto printers = QPrinterInfo::availablePrinters();
    for(auto&& pi : printers)
    {
        ui->comboBoxPrinters->addItem(pi.printerName(), QVariant::fromValue(pi));
        if(_printer && pi.printerName() == _printer->printerName())
            ui->comboBoxPrinters->setCurrentText(pi.printerName());
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!printers.isEmpty());

    connect(ui->radioButtonPortrait, &QRadioButton::toggled, this, &DialogPrintSettings::orientationToggled);
    connect(ui->radioButtonLandscape, &QRadioButton::toggled, this, &DialogPrintSettings::orientationToggled);
}

///
/// \brief DialogPrintSettings::~DialogPrintSettings
///
DialogPrintSettings::~DialogPrintSettings()
{
    delete ui;
}

///
/// \brief DialogPrintSettings::accept
///
void DialogPrintSettings::accept()
{
    if(_printer)
    {
        _printer->setPrinterName(ui->comboBoxPrinters->currentText());
        _printer->setPageOrientation(ui->radioButtonLandscape->isChecked() ? QPageLayout::Landscape : QPageLayout::Portrait);
        _printer->setPageSize(ui->comboBoxSize->currentData().value<QPageSize>());
    }

    QFixedSizeDialog::accept();
}

///
/// \brief DialogPrintSettings::on_comboBoxPrinters_currentIndexChanged
/// \param index
///
void DialogPrintSettings::on_comboBoxPrinters_currentIndexChanged(int index)
{
    const auto name = ui->comboBoxPrinters->itemText(index);
    const auto pi = ui->comboBoxPrinters->itemData(index).value<QPrinterInfo>();
    const QPrinter printer(pi);

    switch(pi.state())
    {
        case QPrinter::Idle:
            ui->labelPrinterStatus->setText(tr("Idle"));
        break;
        case QPrinter::Active:
            ui->labelPrinterStatus->setText(tr("Active"));
        break;
        case QPrinter::Aborted:
            ui->labelPrinterStatus->setText(tr("Aborted"));
        break;
        case QPrinter::Error:
            ui->labelPrinterStatus->setText(tr("Error"));
        break;
    }

    ui->labelPrinterLocation->setText(pi.location());
    ui->labelPrinterType->setText(pi.makeAndModel());

    ui->comboBoxSize->clear();
    const auto supportedPageSizes = pi.supportedPageSizes();
    for(auto&& sz : supportedPageSizes)
        ui->comboBoxSize->addItem(sz.name(), QVariant::fromValue(sz));

    const auto sz = printer.pageLayout().pageSize();
    ui->comboBoxSize->setCurrentText(sz.name());
    ui->comboBoxSize->setEnabled(!supportedPageSizes.empty());


    ui->comboBoxSources->clear();
    const auto papreSources = printer.supportedPaperSources();
    for(auto&& ps : papreSources)
    {
        QString name;
        switch(ps)
        {
            case QPrinter::OnlyOne:
                name = tr("Only One");
            break;

            case QPrinter::Lower:
                name = tr("Lower");
            break;

            case QPrinter::Middle:
                name = tr("Middle");
            break;

            case QPrinter::Manual:
                name = tr("Manual");
            break;

            case QPrinter::Envelope:
                name = tr("Envelope");
            break;

            case QPrinter::EnvelopeManual:
                name = tr("Envelope Manual");
            break;

            case QPrinter::Auto:
                name = tr("Auto");
            break;

            case QPrinter::Tractor:
                name = tr("Tractor");
            break;

            case QPrinter::SmallFormat:
                name = tr("Small Format");
            break;

            case QPrinter::LargeFormat:
                name = tr("Large Format");
            break;

            case QPrinter::LargeCapacity:
                name = tr("Large Capacity");
            break;

            case QPrinter::Cassette:
                name = tr("Cassette");
            break;

            case QPrinter::MaxPageSource:
                name = tr("Max Page Source");
            break;

            case QPrinter::CustomSource:
                name = tr("Custom Source");
            break;

            default:
            break;
        }
        if(name.isEmpty()) continue;
        ui->comboBoxSources->addItem(name, ps);
    }
    const auto ps = printer.paperSource();
    ui->comboBoxSources->setCurrentIndex(ui->comboBoxSources->findData(ps));
    ui->comboBoxSources->setEnabled(!papreSources.empty());

    switch (printer.pageLayout().orientation())
    {
        case QPageLayout::Portrait:
            ui->radioButtonLandscape->setChecked(false);
            ui->radioButtonPortrait->setChecked(true);
            orientationToggled(true);
        break;
        case QPageLayout::Landscape:
            ui->radioButtonLandscape->setChecked(true);
            ui->radioButtonPortrait->setChecked(false);
            orientationToggled(true);
        break;
    }
}

///
/// \brief DialogPrintSettings::orientationToggled
/// \param checked
///
void DialogPrintSettings::orientationToggled(int checked)
{
    if (!checked)
    {
        return;
    }

    if(ui->radioButtonLandscape->isChecked())
    {
        //ui->orientation_icon->setPixmap(UiIcon("iconLandscape").pixmap({64, 64}));
    }
    else if(ui->radioButtonPortrait->isChecked())
    {
        //ui->orientation_icon->setPixmap(UiIcon("iconPortrait").pixmap({64, 64}));
    }
}
