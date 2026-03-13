#ifndef DIALOGMASKWRITEREGITER_H
#define DIALOGMASKWRITEREGITER_H

#include "qfixedsizedialog.h"
#include "displaydefinition.h"
#include "modbuswriteparams.h"
#include "checkablegroupbox.h"
#include "bitpatterncontrol.h"

namespace Ui {
class DialogMaskWriteRegiter;
}

class DialogMaskWriteRegiter : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogMaskWriteRegiter(ModbusMaskWriteParams& params, const DisplayDefinition& dd, QWidget *parent = nullptr);
    ~DialogMaskWriteRegiter();

    void accept() override;

private:
    Ui::DialogMaskWriteRegiter *ui;
    ModbusMaskWriteParams& _writeParams;
};

#endif // DIALOGMASKWRITEREGITER_H
