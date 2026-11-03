#include "customlineedit.h"

///
/// \brief CustomLineEdit::CustomLineEdit
/// \param parent
///
CustomLineEdit::CustomLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
}

///
/// \brief CustomLineEdit::setText
/// \param text
///
void CustomLineEdit::setText(const QString& text)
{
    QLineEdit::setText(text);
    emit textChanged(text);
}
