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

    if(parentWidget() != nullptr)
    {
        QRect parentRect(parentWidget()->mapToGlobal(QPoint(0, 0)), parentWidget()->size());
        move(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parentRect).topLeft());
    }
}
