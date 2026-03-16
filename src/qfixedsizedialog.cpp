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

        if(auto pw = parentWidget() ? parentWidget()->window() : nullptr)
        {
            const auto center = pw->mapToGlobal(pw->rect().center());
            setGeometry(center.x() - width() / 2, center.y() - height() / 2, width(), height());
        }
    }
}
