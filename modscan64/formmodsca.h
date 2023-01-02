#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include <QModbusClient>
#include "enums.h"
#include "displaydefinition.h"
#include "modbuswriteparams.h"

class MainWindow;

namespace Ui {
class FormModSca;
}

///
/// \brief The FormModSca class
///
class FormModSca : public QWidget
{
    Q_OBJECT

public:
    explicit FormModSca(int num, QModbusClient* client, MainWindow* parent);
    ~FormModSca();

    DisplayDefinition displayDefinition() const;
    void setDisplayDefinition(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    void resetCtrs();

private slots:
    void on_timeout();
    void on_readReply();
    void on_lineEditAddress_valueChanged(int);
    void on_lineEditLength_valueChanged(int);
    void on_lineEditDeviceId_valueChanged(int);
    void on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_outputWidget_itemDoubleClicked(quint32 addr, const QVariant& value);

private:
    QModbusRequest createReadRequest();
    void sendReadRequest(const QModbusRequest& request, uint id);
    void writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params);

private:
    Ui::FormModSca *ui;
    QTimer _timer;
    QModbusClient* _modbusClient;
};

#endif // FORMMODSCA_H
