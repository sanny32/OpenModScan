#include <QFile>
#include <QApplication>
#include <QPlainTextEdit>
#include "dialogabout.h"
#include "ui_dialogabout.h"

///
/// \brief arch
/// \return
///
QString arch()
{
#if defined(Q_PROCESSOR_X86_64)
    return "x64";
#elif defined(Q_PROCESSOR_X86)
    return "x86";
#elif defined(Q_PROCESSOR_ARM)
    return "ARM";
#elif defined(Q_PROCESSOR_ARM64)
    return "ARM64";
#else
    return DialogAbout::tr("Unknown");
#endif
}

///
/// \brief DialogAbout::DialogAbout
/// \param parent
///
DialogAbout::DialogAbout(QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    setWindowTitle(tr("About %1...").arg(APP_NAME));
    ui->labelName->setText(APP_NAME);
    ui->labelVersion->setText(tr("Version: %1").arg(APP_VERSION));

    ui->labelArch->setText(tr("• Architecture: %1").arg(arch()));
    ui->labelPlatform->setText(tr("• Platform: %1 %2").arg(QApplication::platformName(), QSysInfo::currentCpuArchitecture()));
    ui->labelQtFramework->setText(tr("• Qt %1 (build with version %2)").arg(qVersion(), QT_VERSION_STR));

    ui->tabWidget->setCurrentIndex(0);
}

///
/// \brief DialogAbout::~DialogAbout
///
DialogAbout::~DialogAbout()
{
    delete ui;
}

///
/// \brief DialogAbout::on_labelLicense_clicked
///
void DialogAbout::on_labelLicense_clicked()
{
    QString license;
    QFile f(":/res/license.txt");
    if(f.open(QFile::ReadOnly))
        license = f.readAll();

    if(license.isEmpty())
        return;

    auto dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose, true);
    dlg->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    dlg->setWindowTitle(QString(tr("License Agreement - %1")).arg(APP_NAME));
    dlg->resize({ 530, 415});

    auto buttonBox = new QDialogButtonBox(dlg);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    QObject::connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    auto textEdit = new QPlainTextEdit(dlg);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(license);

    auto layout = new QVBoxLayout(dlg);
    layout->addWidget(textEdit);
    layout->addWidget(buttonBox);

    dlg->setLayout(layout);
    dlg->show();
}
