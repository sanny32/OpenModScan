#ifndef FUNCTIONCODECOMBOBOX_H
#define FUNCTIONCODECOMBOBOX_H

#include <QComboBox>
#include <QModbusPdu>
#include "enums.h"

class FunctionCodeComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit FunctionCodeComboBox(QWidget *parent = nullptr);

    QModbusPdu::FunctionCode currentFunctionCode() const;
    void setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    void addItem(QModbusPdu::FunctionCode funcCode);

signals:
    void functionCodeChanged(QModbusPdu::FunctionCode funcCode);

private slots:
    void on_currentIndexChanged(int);

private:
    DataDisplayMode _dataDisplayMode;
};

#endif // FUNCTIONCODECOMBOBOX_H
