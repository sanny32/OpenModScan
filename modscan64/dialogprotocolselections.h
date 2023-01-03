#ifndef DIALOGPROTOCOLSELECTIONS_H
#define DIALOGPROTOCOLSELECTIONS_H

#include "qfixedsizedialog.h"
#include "connectiondetails.h"

namespace Ui {
class DialogProtocolSelections;
}

///
/// \brief The DialogProtocolSelections class
///
class DialogProtocolSelections : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogProtocolSelections(ModbusProtocolSelections& mps, QWidget *parent = nullptr);
    ~DialogProtocolSelections();

    void accept() override;

private:
    Ui::DialogProtocolSelections *ui;

private:
    ModbusProtocolSelections& _protocolSelections;
};

#endif // DIALOGPROTOCOLSELECTIONS_H
