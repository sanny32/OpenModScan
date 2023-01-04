#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
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

    QVector<quint16> data() const;

    void setup(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    bool displayHexAddreses() const;
    void setDisplayHexAddreses(bool on);

    void setStatus(const QString& status);

    void update(const QModbusRequest& request);
    void update(QModbusReply* reply);

signals:
    void itemDoubleClicked(quint32 address, const QVariant& value);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void updateDataWidget(const QModbusDataUnit& data);
    void updateTrafficWidget(bool request, const QModbusPdu& pdu);

private:
    Ui::OutputWidget *ui;

private:
    bool _displayHexAddreses;
    DisplayMode _displayMode;
    DataDisplayMode _dataDisplayMode;
    DisplayDefinition _displayDefinition;
    QModbusDataUnit _lastData;
};

#endif // OUTPUTWIDGET_H
