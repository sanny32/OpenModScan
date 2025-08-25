#ifndef DIALOGMASKWRITEREGITER_H
#define DIALOGMASKWRITEREGITER_H

#include "qfixedsizedialog.h"
#include "modbuswriteparams.h"

namespace Ui {
class DialogMaskWriteRegiter;
}

class DialogMaskWriteRegiter : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogMaskWriteRegiter(ModbusMaskWriteParams& params, bool hexAddress, QWidget *parent = nullptr);
    ~DialogMaskWriteRegiter();

    void accept() override;

private:
    Ui::DialogMaskWriteRegiter *ui;
    ModbusMaskWriteParams& _writeParams;
};

#endif // DIALOGMASKWRITEREGITER_H
