#include <QFile>
#include <QScrollArea>
#include <QApplication>
#include <QPlainTextEdit>
#include "aboutdatawidget.h"
#include "dialogabout.h"
#include "ui_dialogabout.h"

#ifdef Q_OS_WIN
#include <windows.h>

typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

QString windowsPrettyName()
{
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (!hMod)
        return "Windows";

    auto rtlGetVersion =
        reinterpret_cast<RtlGetVersionPtr>(
            ::GetProcAddress(hMod, "RtlGetVersion"));

    if (!rtlGetVersion)
        return "Windows";

    RTL_OSVERSIONINFOW rovi = {};
    rovi.dwOSVersionInfoSize = sizeof(rovi);

    if (rtlGetVersion(&rovi) != 0)
        return "Windows";

    const DWORD build = rovi.dwBuildNumber;

    QString name;
    if (rovi.dwMajorVersion == 10 && build >= 22000)
        name = "Windows 11";
    else if (rovi.dwMajorVersion == 10)
        name = "Windows 10";
    else
        name = QString("Windows %1").arg(rovi.dwMajorVersion);

    return DialogAbout::tr("%1 build %2").arg(name, QString::number(build));
}
#endif

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
    ui->labelVersion->setText(tr("Version: <b>%1</b> %2").arg(APP_VERSION, arch()));

    {
        auto vboxLayout = new QVBoxLayout();
        vboxLayout->setContentsMargins(0, 0, 0, 0);

        addComponent(vboxLayout,
                     "Qt",
                     tr("Using %1 and built against %2").arg(qVersion(), QT_VERSION_STR),
                     tr("Cross-platform application development framework."),
                     "https://www.qt.io");

#ifdef Q_OS_LINUX
        addComponent(vboxLayout,
                     QSysInfo::prettyProductName(),
                     QApplication::platformName(),
                     tr("Underlying platform."));
#endif

#ifdef Q_OS_WIN
        addComponent(vboxLayout,
                     windowsPrettyName(),
                     QSysInfo::currentCpuArchitecture(),
                     tr("Underlying platform."));
#endif

        vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        ui->scrollAreaComponentsWidget->setLayout(vboxLayout);
    }

    {
        auto vboxLayout = new QVBoxLayout();
        vboxLayout->setContentsMargins(0, 0, 0, 0);

        addAuthor(vboxLayout,
                  "Alexandr Ananev",
                  tr("Author and Maintainer"),
                  "mailto: mail@ananev.org");

        vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        ui->scrollAreaAuthorsWidget->setLayout(vboxLayout);
    }

    {
        auto vboxLayout = new QVBoxLayout();
        vboxLayout->setContentsMargins(0, 0, 0, 0);

        addAuthor(vboxLayout,
                  "Alexandr Ananev",
                  tr("Russian"));

        addAuthor(vboxLayout,
                  "zx12864",
                  tr("Traditional  Chinese"),
                  "https://github.com/zx12864");

        addAuthor(vboxLayout,
                  "FruitJelliesGD",
                  tr("Simplified Chinese"),
                  "https://github.com/FruitJelliesGD");

        addAuthor(vboxLayout,
                  "lilixxs",
                  tr("Simplified Chinese"),
                  "https://github.com/lilixxs");

        vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        ui->scrollAreaTranslationWidget->setLayout(vboxLayout);

    }

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
/// \brief DialogAbout::sizeHint
/// \return
///
QSize DialogAbout::sizeHint() const
{
    return QSize(400, 320);
}

///
/// \brief DialogAbout::addComponent
/// \param layout
/// \param title
/// \param version
/// \param description
/// \param url
///
void DialogAbout::addComponent(QLayout* layout, const QString& title, const QString& version, const QString& description, const QString& url)
{
    auto w = new AboutDataWidget(this);
    w->setTitle(title);
    w->setVersion(version);
    w->setDescription(description);
    w->setLinkUrl(QUrl(url));
    w->setLinkIcon(QIcon::fromTheme("applications-internet", QIcon(":/res/applications-internet.svg")));
    w->setLinkToolTip(tr("Visit component's homepage\n%1").arg(w->linkUrl().toString()));
    layout->addWidget(w);
}

///
/// \brief DialogAbout::addAuthor
/// \param layout
/// \param name
/// \param description
/// \param url
///
void DialogAbout::addAuthor(QLayout* layout, const QString& name, const QString& description, const QString& url)
{
    auto w = new AboutDataWidget(this);
    w->setTitle(name);
    w->setDescription(description);
    w->setLinkUrl(QUrl(url));

    if(url.contains("mailto:"))
    {
        w->setLinkIcon(QIcon::fromTheme("emblem-mail", QIcon(":/res/emblem-mail.svg")));
        w->setLinkToolTip(tr("Email contributer: %1").arg(w->linkUrl().path()));
    }
    else if(url.contains("github"))
    {
        w->setLinkIcon(QIcon(":/res/emblem-github.svg"));
        w->setLinkToolTip(tr("Visit github user's homepage\n%1").arg(w->linkUrl().toString()));
    }
    else if(!url.isEmpty())
    {
        w->setLinkToolTip(tr("Visit user's homepage\n%1").arg(w->linkUrl().toString()));
    }

    layout->addWidget(w);
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
