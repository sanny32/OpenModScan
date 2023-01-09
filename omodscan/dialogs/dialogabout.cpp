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
    ui->labelVersion->setText(QString("Version: %1").arg(qApp->applicationVersion()));
}

///
/// \brief DialogAbout::~DialogAbout
///
DialogAbout::~DialogAbout()
{
    delete ui;
}
