#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include <QPrinter>
#include "enums.h"
#include "modbusclient.h"
#include "displaydefinition.h"

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
    explicit FormModSca(int id, ModbusClient& client, MainWindow* parent);
    ~FormModSca();

    int formId() const { return _formId; }

    QString filename() const;
    void setFilename(const QString& filename);

    QVector<quint16> data() const;

    DisplayDefinition displayDefinition() const;
    void setDisplayDefinition(const DisplayDefinition& dd);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    bool displayHexAddresses() const;
    void setDisplayHexAddresses(bool on);

    CaptureMode captureMode() const;
    void startTextCapture(const QString& file);
    void stopTextCapture();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& clr);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor& clr);

    QColor statusColor() const;
    void setStatusColor(const QColor& clr);

    QFont font() const;
    void setFont(const QFont& font);

    void print(QPrinter* painter);

    void resetCtrs();
    uint numberOfPolls() const;
    uint validSlaveResposes() const;

public slots:
    void show();

signals:
    void formShowed();
    void numberOfPollsChanged(uint value);
    void validSlaveResposesChanged(uint value);

private slots:
    void on_timeout();
    void on_modbusReply(QModbusReply* reply);
    void on_modbusRequest(int requestId, const QModbusRequest& request);
    void on_lineEditAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditDeviceId_valueChanged(const QVariant&);
    void on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_outputWidget_itemDoubleClicked(quint32 addr, const QVariant& value);
    void on_statisticWidget_numberOfPollsChanged(uint value);
    void on_statisticWidget_validSlaveResposesChanged(uint value);

private:
    bool isValidReply(const QModbusReply* reply);

private:
    Ui::FormModSca *ui;
    int _formId;
    QTimer _timer;
    QString _filename;
    ModbusClient& _modbusClient;
};

#endif // FORMMODSCA_H
