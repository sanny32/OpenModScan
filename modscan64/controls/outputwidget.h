#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include <QModbusReply>
#include "enums.h"
#include "displaydefinition.h"

namespace Ui {
class OutputWidget;
}

///
/// \brief The OutputWidget class
///
class OutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *parent = nullptr);
    ~OutputWidget();

    void setup(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    void setStatus(const QString& status);
    void update(const QModbusRequest& request);
    void update(QModbusReply* reply);

private:
    void updateDataWidget(const QModbusDataUnit& data = QModbusDataUnit());
    void updateTrafficWidget(bool request, const QModbusPdu& pdu);

private:
    Ui::OutputWidget *ui;

private:
    DisplayMode _displayMode;
    DataDisplayMode _dataDisplayMode;
    DisplayDefinition _displayDefinition;
};

#endif // OUTPUTWIDGET_H
