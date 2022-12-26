#ifndef DIALOGCONNECTIONDETAILS_H
#define DIALOGCONNECTIONDETAILS_H

#include <QDialog>
#include "connectiondetails.h"

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
    explicit DialogConnectionDetails(ConnectionDetails& cd, QWidget *parent = nullptr);
    ~DialogConnectionDetails();

    void accept() override;

private slots:
    void on_comboBoxConnectUsing_currentIndexChanged(int);

private:
    Ui::DialogConnectionDetails *ui;

private:
    ConnectionDetails& _connectionDetails;
};

#endif // DIALOGCONNECTIONDETAILS_H
