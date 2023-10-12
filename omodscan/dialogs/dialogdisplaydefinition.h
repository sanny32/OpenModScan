#ifndef DIALOGDISPLAYDEFINITION_H
#define DIALOGDISPLAYDEFINITION_H

#include "qfixedsizedialog.h"
#include "displaydefinition.h"

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
    explicit DialogDisplayDefinition(DisplayDefinition dd, QWidget* parent);
    ~DialogDisplayDefinition();

    DisplayDefinition displayDefinition() const {
        return _displayDefinition;
    }

    void accept() override;

private:
    DisplayDefinition _displayDefinition;
    Ui::DialogDisplayDefinition *ui;
};

#endif // DIALOGDISPLAYDEFINITION_H
