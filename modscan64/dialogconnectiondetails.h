#ifndef DIALOGCONNECTIONDETAILS_H
#define DIALOGCONNECTIONDETAILS_H

#include <QDialog>

namespace Ui {
class DialogConnectionDetails;
}

///
/// \brief The DialogConnectionDetails class
///
class DialogConnectionDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConnectionDetails(QWidget *parent = nullptr);
    ~DialogConnectionDetails();

private slots:
    void on_comboBoxConnectUsing_currentIndexChanged(int);

private:
    Ui::DialogConnectionDetails *ui;
};

#endif // DIALOGCONNECTIONDETAILS_H
