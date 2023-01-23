#ifndef QFIXEDSIZEDIALOG_H
#define QFIXEDSIZEDIALOG_H

#include <QDialog>

///
/// \brief The QFixedSizeDialog class
///
class QFixedSizeDialog : public QDialog
{
    Q_OBJECT

public:
    QFixedSizeDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    void showEvent(QShowEvent* e) override;
};

#endif // QFIXEDSIZEDIALOG_H
