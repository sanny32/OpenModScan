#include <QPrinterInfo>
#include <QPushButton>
#include "dialogprintsettings.h"
#include "ui_dialogprintsettings.h"

Q_DECLARE_METATYPE(QPrinterInfo)
Q_DECLARE_METATYPE(QPrinter::PaperSource)

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

    connect(ui->radioButtonPortrait, &QRadioButton::clicked, this, &DialogPrintSettings::orientationChanged);
    connect(ui->radioButtonLandscape, &QRadioButton::clicked, this, &DialogPrintSettings::orientationChanged);

    const auto printers = QPrinterInfo::availablePrinters();
    for(auto&& pi : printers)
        ui->comboBoxPrinters->addItem(pi.printerName(), QVariant::fromValue(pi));

    if(_printer) ui->comboBoxPrinters->setCurrentText(_printer->printerName());
    else ui->comboBoxPrinters->setCurrentText(QPrinterInfo::defaultPrinterName());

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!printers.isEmpty());
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
        if(ui->comboBoxSources->isEnabled())
            _printer->setPaperSource(ui->comboBoxSources->currentData().value<QPrinter::PaperSource>());
        if(ui->comboBoxSize->isEnabled())
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

    QSharedPointer<QPrinter> printer;
    if(_printer && name == _printer->printerName())
        printer = QSharedPointer<QPrinter>(_printer, [](QPrinter*) {});
    else
        printer = QSharedPointer<QPrinter>(new QPrinter(pi));

    switch(pi.state())
    {
        case QPrinter::Idle:
            ui->labelPrinterStatus->setText(tr("Ready"));
        break;
        case QPrinter::Active:
            ui->labelPrinterStatus->setText(tr("Printing"));
        break;
        case QPrinter::Aborted:
            ui->labelPrinterStatus->setText(tr("Aborted"));
        break;
        case QPrinter::Error:
            ui->labelPrinterStatus->setText(tr("Error"));
        break;
    }

    ui->labelPrinterLocation->setText(pi.location().trimmed());
    ui->labelPrinterType->setText(pi.makeAndModel().trimmed());

    ui->comboBoxSize->clear();
    const auto supportedPageSizes = pi.supportedPageSizes();
    for(auto&& sz : supportedPageSizes)
        ui->comboBoxSize->addItem(sz.name(), QVariant::fromValue(sz));

    const auto sz = printer->pageLayout().pageSize();
    ui->comboBoxSize->setCurrentText(sz.name());
    ui->comboBoxSize->setEnabled(!supportedPageSizes.empty());
    ui->labelSize->setEnabled(ui->comboBoxSize->isEnabled());

    ui->comboBoxSources->clear();
    const auto papreSources = supportedPaperSources(printer.get());
    for(auto&& ps : papreSources)
        ui->comboBoxSources->addItem(ps.Name, ps.Source);

    const auto ps = printer->paperSource();
    ui->comboBoxSources->setCurrentIndex(ui->comboBoxSources->findData(ps));
    ui->comboBoxSources->setEnabled(!papreSources.empty());
    ui->labelSource->setEnabled(ui->comboBoxSources->isEnabled());

    switch (printer->pageLayout().orientation())
    {
        case QPageLayout::Portrait:
            ui->radioButtonPortrait->click();
        break;
        case QPageLayout::Landscape:
            ui->radioButtonLandscape->click();
        break;
    }
}

///
/// \brief DialogPrintSettings::orientationChanged
///
void DialogPrintSettings::orientationChanged()
{
    ui->labelOrientation->setPixmap(QIcon(ui->radioButtonLandscape->isChecked() ?
                                              ":/res/iconLandscape.png" :
                                              ":/res/iconPortrait.png").pixmap({32, 32}));
}

///
/// \brief DialogPrintSettings::supportedPaperSources
/// \param printer
/// \return
///
QList<PapeSource> DialogPrintSettings::supportedPaperSources(const QPrinter* printer)
{
    if(!printer) return QList<PapeSource>();

    QList<PapeSource> listPaperSource;

#if defined(Q_OS_WIN) || defined(Q_CLANG_QDOC)
    const auto papreSources = printer->supportedPaperSources();
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
        listPaperSource.push_back({name, ps});
    }
#endif

    return listPaperSource;
}
