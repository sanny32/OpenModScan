#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include <QPrinter>
#include <QVersionNumber>
#include "enums.h"
#include "modbusclient.h"
#include "datasimulator.h"
#include "displaydefinition.h"
#include "modbussimulationparams.h"

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

    friend QDataStream& operator <<(QDataStream& out, const FormModSca* frm);
    friend QDataStream& operator >>(QDataStream& in, FormModSca* frm);

public:
    static QVersionNumber VERSION;

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

    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder order);

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

    void resumeSimulations();
    void pauseSimulations();
    void restartSimulations();

public slots:
    void show();

signals:
    void showed();
    void byteOrderChanged(ByteOrder);
    void numberOfPollsChanged(uint value);
    void validSlaveResposesChanged(uint value);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_timeout();
    void on_modbusReply(QModbusReply* reply);
    void on_modbusRequest(int requestId, const QModbusRequest& request);
    void on_modbusConnected(const ConnectionDetails& cd);
    void on_modbusDisconnected(const ConnectionDetails& cd);
    void on_dataSimulated(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, QVariant value);
    void on_lineEditAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditDeviceId_valueChanged(const QVariant&);
    void on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_outputWidget_itemDoubleClicked(quint16 addr, const QVariant& value);
    void on_statisticWidget_numberOfPollsChanged(uint value);
    void on_statisticWidget_validSlaveResposesChanged(uint value);

private:
    bool isValidReply(const QModbusReply* reply);

private:
    Ui::FormModSca *ui;
    int _formId;
    uint _validSlaveResponses;
    uint _noSlaveResponsesCounter;
    QTimer _timer;
    QString _filename;
    ModbusClient& _modbusClient;

    QSharedPointer<DataSimulator> _dataSimulator;
    QMap<QPair<QModbusDataUnit::RegisterType, quint16>, ModbusSimulationParams> _simulationMap;
};

///
/// \brief operator <<
/// \param out
/// \param frm
/// \return
///
inline QSettings& operator <<(QSettings& out, const FormModSca* frm)
{
    if(!frm) return out;

    out.setValue("Font", frm->font());
    out.setValue("ForegroundColor", frm->foregroundColor());
    out.setValue("BackgroundColor", frm->backgroundColor());
    out.setValue("StatusColor", frm->statusColor());

    const auto wnd = frm->parentWidget();
    out.setValue("ViewMaximized", wnd->isMaximized());
    if(!wnd->isMaximized() && !wnd->isMinimized())
    {
        out.setValue("ViewSize", wnd->size());
    }

    out << frm->displayMode();
    out << frm->dataDisplayMode();
    out << frm->byteOrder();
    out << frm->displayDefinition();
    out.setValue("DisplayHexAddresses", frm->displayHexAddresses());

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param frm
/// \return
///
inline QSettings& operator >>(QSettings& in, FormModSca* frm)
{
    if(!frm) return in;

    DisplayMode displayMode;
    in >> displayMode;

    DataDisplayMode dataDisplayMode;
    in >> dataDisplayMode;

    ByteOrder byteOrder;
    in >> byteOrder;

    DisplayDefinition displayDefinition;
    in >> displayDefinition;

    bool isMaximized;
    isMaximized = in.value("ViewMaximized").toBool();

    QSize wndSize;
    wndSize = in.value("ViewSize").toSize();

    auto wnd = frm->parentWidget();
    frm->setFont(in.value("Font", wnd->font()).value<QFont>());
    frm->setForegroundColor(in.value("ForegroundColor", QColor(Qt::black)).value<QColor>());
    frm->setBackgroundColor(in.value("BackgroundColor", QColor(Qt::lightGray)).value<QColor>());
    frm->setStatusColor(in.value("StatusColor", QColor(Qt::red)).value<QColor>());

    wnd->resize(wndSize);
    if(isMaximized) wnd->setWindowState(Qt::WindowMaximized);

    frm->setDisplayMode(displayMode);
    frm->setDataDisplayMode(dataDisplayMode);
    frm->setByteOrder(byteOrder);
    frm->setDisplayDefinition(displayDefinition);
    frm->setDisplayHexAddresses(in.value("DisplayHexAddresses").toBool());

    return in;
}

///
/// \brief operator <<
/// \param out
/// \param frm
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const FormModSca* frm)
{
    if(!frm) return out;

    out << frm->formId();

    const auto wnd = frm->parentWidget();
    out << wnd->isMaximized();
    out << ((wnd->isMinimized() || wnd->isMaximized()) ?
              wnd->sizeHint() : wnd->size());

    out << frm->displayMode();
    out << frm->dataDisplayMode();
    out << frm->displayHexAddresses();

    out << frm->backgroundColor();
    out << frm->foregroundColor();
    out << frm->statusColor();
    out << frm->font();

    const auto dd = frm->displayDefinition();
    out << dd.ScanRate;
    out << dd.DeviceId;
    out << dd.PointType;
    out << dd.PointAddress;
    out << dd.Length;

    out << frm->byteOrder();
    out << frm->_simulationMap;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param frm
/// \return
///
inline QDataStream& operator >>(QDataStream& in, FormModSca* frm)
{
    if(!frm) return in;

    bool isMaximized;
    in >> isMaximized;

    QSize windowSize;
    in >> windowSize;

    DisplayMode displayMode;
    in >> displayMode;

    DataDisplayMode dataDisplayMode;
    in >> dataDisplayMode;

    bool hexAddresses;
    in >> hexAddresses;

    QColor bkgClr;
    in >> bkgClr;

    QColor fgClr;
    in >> fgClr;

    QColor stCrl;
    in >> stCrl;

    QFont font;
    in >> font;

    DisplayDefinition dd;
    in >> dd.ScanRate;
    in >> dd.DeviceId;
    in >> dd.PointType;
    in >> dd.PointAddress;
    in >> dd.Length;

    ByteOrder byteOrder = ByteOrder::LittleEndian;
    const auto ver = frm->property("Version").value<QVersionNumber>();
    if(ver >= QVersionNumber(1, 1))
    {
        in >> byteOrder;
        in >> frm->_simulationMap;
    }

    if(in.status() != QDataStream::Ok)
        return in;

    auto wnd = frm->parentWidget();
    wnd->resize(windowSize);
    wnd->setWindowState(Qt::WindowActive);
    if(isMaximized) wnd->setWindowState(Qt::WindowMaximized);

    frm->setDisplayMode(displayMode);
    frm->setDataDisplayMode(dataDisplayMode);
    frm->setDisplayHexAddresses(hexAddresses);
    frm->setBackgroundColor(bkgClr);
    frm->setForegroundColor(fgClr);
    frm->setStatusColor(stCrl);
    frm->setFont(font);
    frm->setDisplayDefinition(dd);
    frm->setByteOrder(byteOrder);
    frm->restartSimulations();

    return in;
}

#endif // FORMMODSCA_H
