#ifndef DIALOGPROTOCOLSELECTIONS_H
#define DIALOGPROTOCOLSELECTIONS_H

#include <QDialog>
#include "connectiondetails.h"

namespace Ui {
class DialogProtocolSelections;
}

///
/// \brief The DialogProtocolSelections class
///
class DialogProtocolSelections : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProtocolSelections(ModbusProtocolSelections& mps, QWidget *parent = nullptr);
    ~DialogProtocolSelections();

    void accept() override;

protected:
    void showEvent(QShowEvent* e) override;

private:
    Ui::DialogProtocolSelections *ui;

private:
    ModbusProtocolSelections& _protocolSelections;
};

#endif // DIALOGPROTOCOLSELECTIONS_H
