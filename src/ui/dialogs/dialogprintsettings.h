#ifndef DIALOGPRINTSETTINGS_H
#define DIALOGPRINTSETTINGS_H

#include <QPrinter>
#include "qfixedsizedialog.h"


namespace Ui {
class DialogPrintSettings;
}

struct PapeSource
{
    QString Name;
    QPrinter::PaperSource Source;
};

///
/// \brief The DialogPrintSettings class
///
class DialogPrintSettings : public QFixedSizeDialog
 {
    Q_OBJECT

public:
    explicit DialogPrintSettings(QPrinter* printer, QWidget *parent = nullptr);
    ~DialogPrintSettings() override;

    void accept() override;

private slots:
    void on_comboBoxPrinters_currentIndexChanged(int index);
    void orientationChanged();

private:
    QList<PapeSource> supportedPaperSources(const QPrinter* printer);

private:
    Ui::DialogPrintSettings *ui;    
    QPrinter* _printer;
};

#endif // DIALOGPRINTSETTINGS_H
