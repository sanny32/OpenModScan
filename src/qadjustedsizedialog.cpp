#include "qadjustedsizedialog.h"

///
/// \brief QAdjustedSizeDialog::QAdjustedSizeDialog
/// \param parent
/// \param f
///
QAdjustedSizeDialog::QAdjustedSizeDialog(QWidget *parent, Qt::WindowFlags f)
    :QDialog(parent, f)
{
    setWindowModality(Qt::WindowModal);
}

///
/// \brief QAdjustedSizeDialog::showEvent
/// \param e
///
void QAdjustedSizeDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);

    if(!_shown) {
        _shown = true;

        adjustSize();
        if(parentWidget() != nullptr)
        {
            const auto rc = parentWidget()->frameGeometry();
            move(rc.center() - rect().center());
        }
    }
}
