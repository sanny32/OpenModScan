#ifndef DIALOGSETUPPRESETDATA_H
#define DIALOGSETUPPRESETDATA_H

#include "qfixedsizedialog.h"
#include "displaydefinition.h"

namespace Ui {
class DialogSetupPresetData;
}

class DialogSetupPresetData : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogSetupPresetData(DisplayDefinition& params, QWidget *parent = nullptr);
    ~DialogSetupPresetData();

    void accept() override;

private:
    Ui::DialogSetupPresetData *ui;
    DisplayDefinition& _params;
};

#endif // DIALOGSETUPPRESETDATA_H
