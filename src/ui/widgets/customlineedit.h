#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>

///
/// \brief The CustomLineEdit class
///
class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    CustomLineEdit(QWidget *parent = nullptr);
    void setText(const QString& text);
};

#endif // CUSTOMLINEEDIT_H
