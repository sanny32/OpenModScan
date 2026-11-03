#ifndef MODBUSPROTOCOLSCOMBOBOX_H
#define MODBUSPROTOCOLSCOMBOBOX_H

#include <QComboBox>

///
/// \brief The ModbusProtocolsComboBox class
///
class ModbusProtocolsComboBox : public QComboBox
{
    Q_OBJECT
public:
    enum ModbusProtocol {
        ModbusTcp,
        ModbusRtuTcp,
        ModbusRtu
    };
    Q_ENUM(ModbusProtocol)

    explicit ModbusProtocolsComboBox(QWidget *parent = nullptr);

    ModbusProtocol currentModbusProtocol() const;
    void setCurrentModbusProtocol(ModbusProtocol mbp);

signals:
    void modbusProtocolChanged(ModbusProtocolsComboBox::ModbusProtocol mbp);

private slots:
    void on_currentIndexChanged(int);
};

#endif // MODBUSPROTOCOLSCOMBOBOX_H
