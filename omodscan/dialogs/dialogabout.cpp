#include <QApplication>
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
    ui->labelName->setText(APP_NAME);
    ui->labelDescription->setText(tr(APP_DESCRIPTION));
    ui->labelVersion->setText(QString(tr("Version: %1")).arg(APP_VERSION));
}

///
/// \brief DialogAbout::~DialogAbout
///
DialogAbout::~DialogAbout()
{
    delete ui;
}
