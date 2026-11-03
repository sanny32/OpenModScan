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
        if(auto pw = parentWidget() ? parentWidget()->window() : nullptr)
        {
            const auto center = pw->mapToGlobal(pw->rect().center());
            setGeometry(center.x() - width() / 2, center.y() - height() / 2, width(), height());
        }
    }
}
