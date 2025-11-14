#include <QFileDialog>
#include <QPushButton>
#include "dialogautostart.h"
#include "ui_dialogautostart.h"

///
/// \brief DialogAutoStart::DialogAutoStart
/// \param filepath
/// \param savePath
/// \param parent
///
DialogAutoStart::DialogAutoStart(QString& filepath, const QString& savePath, QWidget *parent) :
      QFixedSizeDialog(parent)
    , ui(new Ui::DialogAutoStart)
    ,_filepath(filepath)
    ,_savePath(savePath)
{
    ui->setupUi(this);
    ui->lineEditPath->setText(_filepath);
}

///
/// \brief DialogAutoStart::~DialogAutoStart
///
DialogAutoStart::~DialogAutoStart()
{
    delete ui;
}

///
/// \brief DialogAutoStart::accept
///
void DialogAutoStart::accept()
{
    _filepath = ui->lineEditPath->text();
    QFixedSizeDialog::accept();
}

///
/// \brief DialogAutoStart::on_pushButtonBrowse_clicked
///
void DialogAutoStart::on_pushButtonBrowse_clicked()
{
    QStringList filters;
    filters << tr("XML files (*.xml)");
    filters << tr("All files (*)");

    const auto filename = QFileDialog::getOpenFileName(this, QString(), _filepath.isEmpty() ? _savePath : _filepath, filters.join(";;"));
    if(filename.isEmpty()) return;

    ui->lineEditPath->setText(filename);
}

///
/// \brief DialogAutoStart::on_lineEditPath_textChanged
/// \param text
///
void DialogAutoStart::on_lineEditPath_textChanged(const QString& text)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
