#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QTimer>
#include <QPrinter>
#include <QVersionNumber>
#include "enums.h"
#include "fontutils.h"
#include "modbusclient.h"
#include "datasimulator.h"
#include "displaydefinition.h"
#include "outputwidget.h"
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

    explicit FormModSca(int id, ModbusClient& client, DataSimulator* simulator, MainWindow* parent);
    ~FormModSca();

    int formId() const {
        return _formId;
    }

    bool isActive() const {
        return property("isActive").toBool();
    }

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

    QString codepage() const;
    void setCodepage(const QString& name);

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

    ModbusSimulationMap simulationMap() const;
    void startSimulation(QModbusDataUnit::RegisterType type, quint16 addr, const ModbusSimulationParams& params);

    AddressDescriptionMap descriptionMap() const;
    void setDescription(QModbusDataUnit::RegisterType type, quint16 addr, const QString& desc);

    void resetCtrs();
    uint numberOfPolls() const;
    uint validSlaveResposes() const;

    PollState pollState() const;
    void setPollState(PollState state);

public slots:
    void show();

signals:
    void showed();
    void pointTypeChanged(QModbusDataUnit::RegisterType);
    void byteOrderChanged(ByteOrder);
    void codepageChanged(const QString&);
    void numberOfPollsChanged(uint value);
    void validSlaveResposesChanged(uint value);
    void captureError(const QString& error);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void on_timeout();
    void on_modbusConnected(const ConnectionDetails& cd);
    void on_modbusDisconnected(const ConnectionDetails& cd);
    void on_modbusReply(const ModbusReply* const reply);
    void on_modbusRequest(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void on_modbusResponse(int requestGroupId, QSharedPointer<const ModbusMessage> msg);
    void on_lineEditAddress_valueChanged(const QVariant&);
    void on_lineEditLength_valueChanged(const QVariant&);
    void on_lineEditDeviceId_valueChanged(const QVariant&);
    void on_comboBoxAddressBase_addressBaseChanged(AddressBase base);
    void on_comboBoxModbusPointType_pointTypeChanged(QModbusDataUnit::RegisterType);
    void on_outputWidget_itemDoubleClicked(quint16 addr, const QVariant& value);
    void on_statisticWidget_numberOfPollsChanged(uint value);
    void on_statisticWidget_validSlaveResposesChanged(uint value);
    void on_simulationStarted(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);
    void on_simulationStopped(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId);
    void on_dataSimulated(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, QVariant value);

private:
    void beginUpdate();
    bool isValidReply(const ModbusReply* const reply) const;

    void logModbusMessage(int requestGroupId, QSharedPointer<const ModbusMessage> msg);

private:
    Ui::FormModSca *ui;
    int _formId;
    uint _validSlaveResponses;
    uint _noSlaveResponsesCounter;
    QTimer _timer;
    QString _filename;
    ModbusClient& _modbusClient;
    DataSimulator* _dataSimulator;
    MainWindow* _parent;
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

    out.setValue("FormVersion", FormModSca::VERSION.toString());
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
    out.setValue("Codepage", frm->codepage());

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

    QVersionNumber version;
    version = QVersionNumber::fromString(in.value("FormVersion").toString());

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

    if(!version.isNull() || version >= QVersionNumber(1, 7)) {
        frm->setFont(in.value("Font", defaultMonospaceFont()).value<QFont>());
        frm->setForegroundColor(in.value("ForegroundColor", QColor(Qt::black)).value<QColor>());
        frm->setBackgroundColor(in.value("BackgroundColor", QColor(Qt::white)).value<QColor>());
        frm->setStatusColor(in.value("StatusColor", QColor(Qt::red)).value<QColor>());
    }

    auto wnd = frm->parentWidget();
    wnd->resize(wndSize);
    if(isMaximized) wnd->setWindowState(Qt::WindowMaximized);

    frm->setDisplayMode(displayMode);
    frm->setDataDisplayMode(dataDisplayMode);
    frm->setByteOrder(byteOrder);
    frm->setDisplayDefinition(displayDefinition);
    frm->setDisplayHexAddresses(in.value("DisplayHexAddresses").toBool());
    frm->setCodepage(in.value("Codepage").toString());

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
    out << dd.FormName;
    out << dd.ScanRate;
    out << dd.DeviceId;
    out << dd.PointType;
    out << dd.PointAddress;
    out << dd.Length;
    out << dd.LogViewLimit;
    out << dd.ZeroBasedAddress;
    out << dd.DataViewColumnsDistance;
    out << dd.LeadingZeros;

    out << frm->byteOrder();
    out << frm->simulationMap();
    out << frm->descriptionMap();
    out << frm->codepage();

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
    const auto ver = frm->property("Version").value<QVersionNumber>();

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

    if(ver >= QVersionNumber(1, 8))
    {
        in >> dd.FormName;
    }

    in >> dd.ScanRate;
    in >> dd.DeviceId;
    in >> dd.PointType;
    in >> dd.PointAddress;
    in >> dd.Length;
    if(ver >= QVersionNumber(1, 4))
    {
        in >> dd.LogViewLimit;
    }
    if(ver >= QVersionNumber(1, 5))
    {
        in >> dd.ZeroBasedAddress;
    }
    if(ver >= QVersionNumber(1, 8)) {
        in >> dd.DataViewColumnsDistance;
        in >> dd.LeadingZeros;
    }

    ByteOrder byteOrder = ByteOrder::Direct;
    ModbusSimulationMap simulationMap;
    if(ver >= QVersionNumber(1, 1))
    {
        in >> byteOrder;
        in >> simulationMap;
    }

    AddressDescriptionMap descriptionMap;
    if(ver >= QVersionNumber(1, 2))
    {
        in >> descriptionMap;
    }

    QString codepage;
    if(ver >= QVersionNumber(1, 6))
    {
        in >> codepage;
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
    frm->setCodepage(codepage);

    for(auto&& k : simulationMap.keys())
        frm->startSimulation(k.first, k.second,  simulationMap[k]);

    for(auto&& k : descriptionMap.keys())
        frm->setDescription(k.first, k.second, descriptionMap[k]);

    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param frm
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, FormModSca* frm)
{
    if (!frm) return xml;

    xml.writeStartElement("FormModScan");

    xml.writeAttribute("Version", FormModSca::VERSION.toString());
    xml.writeAttribute("DisplayMode", enumToString<DisplayMode>(frm->displayMode()));
    xml.writeAttribute("DataDisplayMode", enumToString<DataDisplayMode>(frm->dataDisplayMode()));
    xml.writeAttribute("Codepage", frm->codepage());
    xml.writeAttribute("ByteOrder", enumToString<ByteOrder>(frm->byteOrder()));

    const auto wnd = frm->parentWidget();
    xml.writeStartElement("Window");
    xml.writeAttribute("Maximized", boolToString(wnd->isMaximized()));
    xml.writeAttribute("Minimized", boolToString(wnd->isMinimized()));

    const auto windowSize = (wnd->isMinimized() || wnd->isMaximized()) ? wnd->sizeHint() : wnd->size();
    xml.writeAttribute("Width", QString::number(windowSize.width()));
    xml.writeAttribute("Height", QString::number(windowSize.height()));
    xml.writeEndElement();

    xml.writeStartElement("Colors");
    xml.writeAttribute("Background", frm->backgroundColor().name());
    xml.writeAttribute("Foreground", frm->foregroundColor().name());
    xml.writeAttribute("Status", frm->statusColor().name());
    xml.writeEndElement();

    xml.writeStartElement("Font");
    const QFont font = frm->font();
    xml.writeAttribute("Family", font.family());
    xml.writeAttribute("Size", QString::number(font.pointSize()));
    xml.writeAttribute("Bold", boolToString(font.bold()));
    xml.writeAttribute("Italic", boolToString(font.italic()));
    xml.writeEndElement();

    const auto dd = frm->displayDefinition();
    xml << dd;

    {
        const auto simulationMap = frm->simulationMap();
        xml.writeStartElement("ModbusSimulationMap");

        for (auto it = simulationMap.constBegin(); it != simulationMap.constEnd(); ++it) {
            const QPair<QModbusDataUnit::RegisterType, quint16>& key = it.key();
            const ModbusSimulationParams& params = it.value();

            if(params.Mode != SimulationMode::Off && key.first == dd.PointType)
            {
                xml.writeStartElement("Simulation");
                xml.writeAttribute("Address", QString::number(key.second + (dd.ZeroBasedAddress ? 0 : 1)));
                xml << params;
                xml.writeEndElement();
            }
        }

        xml.writeEndElement(); // ModbusSimulationMap
    }

    {
        const auto descriptionMap = frm->descriptionMap();
        xml.writeStartElement("AddressDescriptionMap");

        for (auto it = descriptionMap.constBegin(); it != descriptionMap.constEnd(); ++it) {
            const QPair<QModbusDataUnit::RegisterType, quint16>& key = it.key();
            const QString& description = it.value();

            if(!description.isEmpty() && key.first == dd.PointType)
            {
                xml.writeStartElement("Description");
                xml.writeAttribute("Address", QString::number(key.second));
                xml.writeCDATA(description);
                xml.writeEndElement();
            }
        }

        xml.writeEndElement(); // AddressDescriptionMap
    }

    xml.writeEndElement(); // FormModScan

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param frm
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, FormModSca* frm)
{
    if (!frm) return xml;

    if (xml.isStartElement() && xml.name() == QLatin1String("FormModScan")) {
        DataDisplayMode ddm;
        DisplayDefinition dd;
        QHash<quint16, QString> descriptions;
        QHash<quint16, ModbusSimulationParams> simulations;

        const QXmlStreamAttributes attributes = xml.attributes();

        if (attributes.hasAttribute("DisplayMode")) {
            const DisplayMode mode = enumFromString<DisplayMode>(attributes.value("DisplayMode").toString());
            frm->setDisplayMode(mode);
        }

        if (attributes.hasAttribute("DataDisplayMode")) {
            ddm = enumFromString<DataDisplayMode>(attributes.value("DataDisplayMode").toString());
        }

        if (attributes.hasAttribute("Codepage")) {
            frm->setCodepage(attributes.value("Codepage").toString());
        }

        if (attributes.hasAttribute("ByteOrder")) {
            const ByteOrder order = enumFromString<ByteOrder>(attributes.value("ByteOrder").toString());
            frm->setByteOrder(order);
        }

        while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("Window")) {
                const QXmlStreamAttributes windowAttrs = xml.attributes();

                const auto wnd = frm->parentWidget();
                if (wnd) {
                    if (windowAttrs.hasAttribute("Width") && windowAttrs.hasAttribute("Height")) {
                        bool okWidth, okHeight;
                        const int width = windowAttrs.value("Width").toInt(&okWidth);
                        const int height = windowAttrs.value("Height").toInt(&okHeight);

                        if (okWidth && okHeight && !wnd->isMaximized() && !wnd->isMinimized()) {
                            wnd->resize(width, height);
                        }
                    }

                    if (windowAttrs.hasAttribute("Maximized")) {
                        const bool maximized = stringToBool(windowAttrs.value("Maximized").toString());
                        if (maximized) wnd->showMaximized();
                    }

                    if (windowAttrs.hasAttribute("Minimized")) {
                        const bool minimized = stringToBool(windowAttrs.value("Minimized").toString());
                        if (minimized) wnd->showMinimized();
                    }
                }
                xml.skipCurrentElement();
            }
            else if (xml.name() == QLatin1String("Colors")) {
                const QXmlStreamAttributes colorAttrs = xml.attributes();

                if (colorAttrs.hasAttribute("Background")) {
                    QColor color(colorAttrs.value("Background").toString());
                    if (color.isValid()) frm->setBackgroundColor(color);
                }

                if (colorAttrs.hasAttribute("Foreground")) {
                    QColor color(colorAttrs.value("Foreground").toString());
                    if (color.isValid()) frm->setForegroundColor(color);
                }

                if (colorAttrs.hasAttribute("Status")) {
                    QColor color(colorAttrs.value("Status").toString());
                    if (color.isValid()) frm->setStatusColor(color);
                }
                xml.skipCurrentElement();
            }
            else if (xml.name() == QLatin1String("Font")) {
                const QXmlStreamAttributes fontAttrs = xml.attributes();

                QFont font = frm->font();

                if (fontAttrs.hasAttribute("Family")) {
                    font.setFamily(fontAttrs.value("Family").toString());
                }

                if (fontAttrs.hasAttribute("Size")) {
                    bool ok; const int size = fontAttrs.value("Size").toInt(&ok);
                    if (ok && size > 0) font.setPointSize(size);
                }

                if (fontAttrs.hasAttribute("Bold")) {
                    font.setBold(stringToBool(fontAttrs.value("Bold").toString()));
                }

                if (fontAttrs.hasAttribute("Italic")) {
                    font.setItalic(stringToBool(fontAttrs.value("Italic").toString()));
                }

                frm->setFont(font);
                xml.skipCurrentElement();
            }
            else if (xml.name() == QLatin1String("DisplayDefinition")) {
                xml >> dd;
                frm->setDisplayDefinition(dd);
            }
            else if (xml.name() == QLatin1String("ModbusSimulationMap")) {
                while (xml.readNextStartElement()) {
                    if (xml.name() == QLatin1String("Simulation")) {

                        const QXmlStreamAttributes attributes = xml.attributes();
                        bool ok; const quint16 address = attributes.value("Address").toUShort(&ok);

                        if(ok) {
                            xml.readNextStartElement();

                            ModbusSimulationParams params;
                            xml >> params;

                            simulations[address] = params;
                        }

                        xml.skipCurrentElement();

                    } else {
                        xml.skipCurrentElement();
                    }
                }
            }
            else if (xml.name() == QLatin1String("AddressDescriptionMap")) {
                while (xml.readNextStartElement()) {
                    if (xml.name() == QLatin1String("Description")) {

                        const QXmlStreamAttributes attributes = xml.attributes();
                        bool ok; const quint16 address = attributes.value("Address").toUShort(&ok);

                        if(ok) {
                            QString description;
                            if (xml.isCDATA()) {
                                description = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                            } else {
                                description = xml.readElementText();
                            }
                            descriptions[address] = description;
                        }

                    } else {
                        xml.skipCurrentElement();
                    }
                }
            }
            else {
                xml.skipCurrentElement();
            }
        }

        if(dd.PointType != QModbusDataUnit::Invalid) {
            frm->setDataDisplayMode(ddm);

            if(!simulations.isEmpty()) {
                QHashIterator it(simulations);
                while(it.hasNext()) {
                    const auto item = it.next();
                    switch(dd.PointType) {
                    case QModbusDataUnit::Coils:
                    case QModbusDataUnit::DiscreteInputs:
                        if(item->Mode == SimulationMode::Toggle || item->Mode == SimulationMode::Random)
                            frm->startSimulation(dd.PointType, item.key() - (dd.ZeroBasedAddress ? 0 : 1), item.value());
                        break;
                    case QModbusDataUnit::InputRegisters:
                    case QModbusDataUnit::HoldingRegisters:
                        if(item->Mode != SimulationMode::Off && item->Mode != SimulationMode::Toggle)
                            frm->startSimulation(dd.PointType, item.key() - (dd.ZeroBasedAddress ? 0 : 1), item.value());
                        break;
                    default: break;
                    }
                }
            }

            if(!descriptions.isEmpty()) {
                QHashIterator it(descriptions);
                while(it.hasNext()) {
                    const auto item = it.next();
                    frm->setDescription(dd.PointType, item.key(), item.value());
                }
            }
        }
    }
    else {
        xml.skipCurrentElement();
    }

    return xml;
}

#endif // FORMMODSCA_H
