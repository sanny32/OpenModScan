#ifndef DIALOGDISPLAYDEFINITION_H
#define DIALOGDISPLAYDEFINITION_H

#include "qfixedsizedialog.h"
#include "formmodsca.h"

namespace Ui {
class DialogDisplayDefinition;
}

///
/// \brief The DialogDisplayDefinition class
///
class DialogDisplayDefinition : public QFixedSizeDialog
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
