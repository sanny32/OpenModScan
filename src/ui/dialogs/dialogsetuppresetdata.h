#ifndef DIALOGSETUPPRESETDATA_H
#define DIALOGSETUPPRESETDATA_H

#include <QModbusDataUnit>
#include "qfixedsizedialog.h"
#include "displaydefinition.h"

struct SetupPresetParams
{
    quint16 DeviceId;
    quint16 PointAddress;
    quint16 Length;
    bool ZeroBasedAddress;
    bool LeadingZeros = false;
};

namespace Ui {
class DialogSetupPresetData;
}

///
/// \brief The DialogSetupPresetData class
///
class DialogSetupPresetData : public QFixedSizeDialog
{
    Q_OBJECT

public:
    explicit DialogSetupPresetData(SetupPresetParams& params, QModbusDataUnit::RegisterType pointType, const DisplayDefinition& dd, QWidget *parent = nullptr);
    ~DialogSetupPresetData();

    void accept() override;

private slots:
    void on_lineEditAddress_valueChanged(const QVariant&);

private:
    Ui::DialogSetupPresetData *ui;
    SetupPresetParams& _params;
    QModbusDataUnit::RegisterType _pointType;
};

#endif // DIALOGSETUPPRESETDATA_H
