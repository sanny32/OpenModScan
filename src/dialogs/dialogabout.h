#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include "qfixedsizedialog.h"

namespace Ui {
class DialogAbout;
}

///
/// \brief The DialogAbout class
///
class DialogAbout : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = nullptr);
    ~DialogAbout();

private slots:
    void on_labelLicense_clicked();

private:
    void adjustSize();
    void addComponent(QLayout* layout, const QString& title, const QString& version, const QString& description, const QString& url = QString());
    void addAuthor(QLayout* layout, const QString& name, const QString& description, const QString& url = QString());

private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
