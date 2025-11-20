#ifndef DIALOGAUTOSTART_H
#define DIALOGAUTOSTART_H

#include <QLineEdit>
#include "qfixedsizedialog.h"

namespace Ui {
class DialogAutoStart;
}

///
/// \brief The DialogAutoStart class
///
class DialogAutoStart : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogAutoStart(QString& filepath, const QString& savePath, QWidget *parent = nullptr);
    ~DialogAutoStart();

    void accept() override;

private slots:
    void on_pushButtonBrowse_clicked();
    void on_lineEditPath_textChanged(const QString& text);

private:
    Ui::DialogAutoStart *ui;
    QString& _filepath;
    QString _savePath;
};

#endif // DIALOGAUTOSTART_H
