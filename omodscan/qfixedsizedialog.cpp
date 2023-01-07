#include "qfixedsizedialog.h"

///
/// \brief QFixedSizeDialog::QFixedSizeDialog
/// \param parent
/// \param f
///
QFixedSizeDialog::QFixedSizeDialog(QWidget *parent, Qt::WindowFlags f)
    :QDialog(parent, f)
{
    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint);
}

///
/// \brief QFixedSizeDialog::showEvent
/// \param e
///
void QFixedSizeDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);
    setFixedSize(sizeHint());
}
