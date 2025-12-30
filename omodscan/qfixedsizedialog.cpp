#include <QStyle>
#include "qfixedsizedialog.h"

///
/// \brief QFixedSizeDialog::QFixedSizeDialog
/// \param parent
/// \param f
///
QFixedSizeDialog::QFixedSizeDialog(QWidget *parent, Qt::WindowFlags f)
    :QDialog(parent, f)
{
    setWindowModality(Qt::WindowModal);
}

///
/// \brief QFixedSizeDialog::showEvent
/// \param e
///
void QFixedSizeDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);

    if(!_shown) {
        _shown = true;
        setFixedSize(sizeHint());

        if(parentWidget() != nullptr)
        {
            const auto rc = parentWidget()->frameGeometry();
            move(rc.center() - rect().center());
        }
    }
}
