#ifndef DIALOGDISPLAYDEFINITION_H
#define DIALOGDISPLAYDEFINITION_H

#include <QDialog>
#include "formmodsca.h"

namespace Ui {
class DialogDisplayDefinition;
}

class DialogDisplayDefinition : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDisplayDefinition(FormModSca* parent);
    ~DialogDisplayDefinition();

    void accept() override;

private:
    Ui::DialogDisplayDefinition *ui;
};

#endif // DIALOGDISPLAYDEFINITION_H
