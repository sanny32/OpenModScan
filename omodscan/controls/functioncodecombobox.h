#ifndef FUNCTIONCODECOMBOBOX_H
#define FUNCTIONCODECOMBOBOX_H

#include <QComboBox>
#include <QModbusPdu>

class FunctionCodeComboBox : public QComboBox
{
    Q_OBJECT

public:
    FunctionCodeComboBox(QWidget *parent = nullptr);

    QModbusPdu::FunctionCode currentFunctionCode() const;
    void setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode);

signals:
    void functionCodeChanged(QModbusPdu::FunctionCode funcCode);

private slots:
    void on_currentIndexChanged(int);
};

#endif // FUNCTIONCODECOMBOBOX_H
