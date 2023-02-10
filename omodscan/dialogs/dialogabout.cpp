#include <QFile>
#include <QApplication>
#include <QPlainTextEdit>
#include "dialogabout.h"
#include "ui_dialogabout.h"

///
/// \brief DialogAbout::DialogAbout
/// \param parent
///
DialogAbout::DialogAbout(QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    setWindowTitle(QString(tr("About %1...")).arg(APP_NAME));
    ui->labelName->setText(APP_NAME);
    ui->labelVersion->setText(QString(tr("Version: %1")).arg(APP_VERSION));
    ui->labelQtFramework->setText(QString(tr("• Qt %1 (build with version %2)")).arg(qVersion(), QT_VERSION_STR));
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
