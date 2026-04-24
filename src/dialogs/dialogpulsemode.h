#ifndef DIALOGPULSEMODE_H
#define DIALOGPULSEMODE_H

#include <QDialog>

namespace Ui {
class DialogPulseMode;
}

class DialogPulseMode : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPulseMode(QWidget *parent = nullptr);
    ~DialogPulseMode();

private:
    Ui::DialogPulseMode *ui;
};

#endif // DIALOGPULSEMODE_H
