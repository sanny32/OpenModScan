#ifndef DIALOGPULSEMODE_H
#define DIALOGPULSEMODE_H

#include "qfixedsizedialog.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogPulseMode;
}

///
/// \brief The DialogPulseMode class
///
class DialogPulseMode : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogPulseMode(PulseParams& params, QWidget *parent = nullptr);
    ~DialogPulseMode();

    void accept() override;

private:
    Ui::DialogPulseMode *ui;

private:
    PulseParams& _params;
};

#endif // DIALOGPULSEMODE_H
