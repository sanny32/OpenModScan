QT += core gui widgets network printsupport serialbus serialport

CONFIG += c++17
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

VERSION = 1.6.1

QMAKE_TARGET_PRODUCT = "Open ModScan"
QMAKE_TARGET_DESCRIPTION = "An Open Source Modbus Master (Client) Utility"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"
DEFINES += APP_DESCRIPTION=\"\\\"$${QMAKE_TARGET_DESCRIPTION}\\\"\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += res/omodscan.ico

INCLUDEPATH += controls \
               dialogs \
               modbusmessages \

SOURCES += \
    controls/booleancombobox.cpp \
    controls/bytelisttextedit.cpp \
    controls/byteordercombobox.cpp \
    controls/clickablelabel.cpp \
    controls/connectioncombobox.cpp \
    controls/customframe.cpp \
    controls/customlineedit.cpp \
    controls/flowcontroltypecombobox.cpp \
    controls/formattedspinbox.cpp \
    controls/functioncodecombobox.cpp \
    controls/ipaddresslineedit.cpp \
    controls/mainstatusbar.cpp \
    controls/modbuslogwidget.cpp \
    controls/modbusmessagewidget.cpp \
    controls/numericlineedit.cpp \
    controls/paritytypecombobox.cpp \
    controls/simulationmodecombobox.cpp \
    controls/statisticwidget.cpp \
    controls/numericcombobox.cpp \
    controls/outputwidget.cpp \
    controls/pointtypecombobox.cpp \
    datasimulator.cpp \
    dialogs/dialogmsgparser.cpp \
    dialogs/dialogabout.cpp \
    dialogs/dialogaddressscan.cpp \
    dialogs/dialogautosimulation.cpp \
    dialogs/dialogautostart.cpp \
    dialogs/dialogcoilsimulation.cpp \
    dialogs/dialogconnectiondetails.cpp \
    dialogs/dialogdisplaydefinition.cpp \
    dialogs/dialogforcemultiplecoils.cpp \
    dialogs/dialogforcemultipleregisters.cpp \
    dialogs/dialogmaskwriteregiter.cpp \
    dialogs/dialogmodbusscanner.cpp \
    dialogs/dialogprintsettings.cpp \
    dialogs/dialogprotocolselections.cpp \
    dialogs/dialogsetuppresetdata.cpp \
    dialogs/dialogusermsg.cpp \
    dialogs/dialogwindowsmanager.cpp \
    dialogs/dialogwritecoilregister.cpp \
    dialogs/dialogwriteholdingregister.cpp \
    dialogs/dialogwriteholdingregisterbits.cpp \
    formmodsca.cpp \
    htmldelegate.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusclient.cpp \
    modbusdataunit.cpp \
    modbusmessages/modbusmessage.cpp \
    modbusrtuscanner.cpp \
    modbusscanner.cpp \
    modbustcpscanner.cpp \
    qfixedsizedialog.cpp \
    qhexvalidator.cpp \
    qint64validator.cpp \
    quintvalidator.cpp \
    recentfileactionlist.cpp \
    windowactionlist.cpp

HEADERS += \
    byteorderutils.h \
    connectiondetails.h \
    controls/booleancombobox.h \
    controls/bytelisttextedit.h \
    controls/byteordercombobox.h \
    controls/clickablelabel.h \
    controls/connectioncombobox.h \
    controls/customframe.h \
    controls/customlineedit.h \
    controls/flowcontroltypecombobox.h \
    controls/formattedspinbox.h \
    controls/functioncodecombobox.h \
    controls/ipaddresslineedit.h \
    controls/mainstatusbar.h \
    controls/modbuslogwidget.h \
    controls/modbusmessagewidget.h \
    controls/numericlineedit.h \
    controls/paritytypecombobox.h \
    controls/simulationmodecombobox.h \
    controls/statisticwidget.h \
    controls/numericcombobox.h \
    controls/outputwidget.h \
    controls/pointtypecombobox.h \
    datasimulator.h \
    dialogs/dialogmsgparser.h \
    dialogs/dialogabout.h \
    dialogs/dialogaddressscan.h \
    dialogs/dialogautosimulation.h \
    dialogs/dialogautostart.h \
    dialogs/dialogcoilsimulation.h \
    dialogs/dialogconnectiondetails.h \
    dialogs/dialogdisplaydefinition.h \
    dialogs/dialogforcemultiplecoils.h \
    dialogs/dialogforcemultipleregisters.h \
    dialogs/dialogmaskwriteregiter.h \
    dialogs/dialogmodbusscanner.h \
    dialogs/dialogprintsettings.h \
    dialogs/dialogprotocolselections.h \
    dialogs/dialogsetuppresetdata.h \
    dialogs/dialogusermsg.h \
    dialogs/dialogwindowsmanager.h \
    dialogs/dialogwritecoilregister.h \
    dialogs/dialogwriteholdingregister.h \
    dialogs/dialogwriteholdingregisterbits.h \
    displaydefinition.h \
    enums.h \
    formatutils.h \
    formmodsca.h \
    htmldelegate.h \
    mainwindow.h \
    modbusclient.h \
    modbusdataunit.h \
    modbusexception.h \
    modbusfunction.h \
    modbusmessages/diagnostics.h \
    modbusmessages/getcommeventcounter.h \
    modbusmessages/getcommeventlog.h \
    modbusmessages/maskwriteregister.h \
    modbusmessages/modbusmessage.h \
    modbusmessages/modbusmessages.h \
    modbusmessages/readcoils.h \
    modbuslimits.h \
    modbusmessages/readdiscreteinputs.h \
    modbusmessages/readexceptionstatus.h \
    modbusmessages/readfifoqueue.h \
    modbusmessages/readfilerecord.h \
    modbusmessages/readholdingregisters.h \
    modbusmessages/readinputregisters.h \
    modbusmessages/readwritemultipleregisters.h \
    modbusmessages/reportserverid.h \
    modbusmessages/writefilerecord.h \
    modbusmessages/writemultiplecoils.h \
    modbusmessages/writemultipleregisters.h \
    modbusmessages/writesinglecoil.h \
    modbusmessages/writesingleregister.h \
    modbusrtuscanner.h \
    modbusscanner.h \
    modbussimulationparams.h \
    modbustcpscanner.h \
    modbuswriteparams.h \
    numericutils.h \
    qfixedsizedialog.h \
    qhexvalidator.h \
    qint64validator.h \
    qmodbusadu.h \
    qmodbusadurtu.h \
    qmodbusadutcp.h \
    qrange.h \
    quintvalidator.h \
    recentfileactionlist.h \
    windowactionlist.h

FORMS += \
    controls/outputwidget.ui \
    controls/statisticwidget.ui \
    dialogs/dialogmsgparser.ui \
    dialogs/dialogabout.ui \
    dialogs/dialogaddressscan.ui \
    dialogs/dialogautosimulation.ui \
    dialogs/dialogautostart.ui \
    dialogs/dialogcoilsimulation.ui \
    dialogs/dialogconnectiondetails.ui \
    dialogs/dialogdisplaydefinition.ui \
    dialogs/dialogforcemultiplecoils.ui \
    dialogs/dialogforcemultipleregisters.ui \
    dialogs/dialogmaskwriteregiter.ui \
    dialogs/dialogmodbusscanner.ui \
    dialogs/dialogprintsettings.ui \
    dialogs/dialogprotocolselections.ui \
    dialogs/dialogsetuppresetdata.ui \
    dialogs/dialogusermsg.ui \
    dialogs/dialogwindowsmanager.ui \
    dialogs/dialogwritecoilregister.ui \
    dialogs/dialogwriteholdingregister.ui \
    dialogs/dialogwriteholdingregisterbits.ui \
    formmodsca.ui \
    mainwindow.ui


RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translations/omodscan_ru.ts
