#ifndef DIALOGWRITECOILREGISTER_H
#define DIALOGWRITECOILREGISTER_H

#include <QDialog>
#include "modbuswriteparams.h"

namespace Ui {
class DialogWriteCoilRegister;
}

///
/// \brief The DialogWriteCoilRegister class
///
class DialogWriteCoilRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWriteCoilRegister(const ModbusWriteParams& params, QWidget *parent = nullptr);
    ~DialogWriteCoilRegister();

    ModbusWriteParams writeParams() const;

protected:
    void showEvent(QShowEvent* e) override;

private:
    Ui::DialogWriteCoilRegister *ui;
};

#endif // DIALOGWRITECOILREGISTER_H
