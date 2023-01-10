QT += core gui widgets network serialbus serialport
CONFIG += c++17
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

VERSION = 1.0.0b

QMAKE_TARGET_COMPANY = "Open Modscan"
QMAKE_TARGET_PRODUCT = omodscan
QMAKE_TARGET_DESCRIPTION = "Free modbus rtu/tcp master (client) utility"
QMAKE_TARGET_DOMAIN = "com.omodscan.app"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += APP_NAME=\"\\\"$${QMAKE_TARGET_COMPANY}\\\"\"
DEFINES += APP_DOMAIN=\"\\\"$${QMAKE_TARGET_DOMAIN}\\\"\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += res/omodscan.ico

INCLUDEPATH += controls \
               dialogs \

SOURCES += \
    appsettings.cpp \
    controls/booleancombobox.cpp \
    controls/bytelistlineedit.cpp \
    controls/connectioncombobox.cpp \
    controls/flowcontroltypecombobox.cpp \
    controls/numericlineedit.cpp \
    controls/paritytypecombobox.cpp \
    controls/statisticwidget.cpp \
    controls/numericcombobox.cpp \
    controls/outputwidget.cpp \
    controls/pointtypecombobox.cpp \
    dialogs/dialogabout.cpp \
    dialogs/dialogconnectiondetails.cpp \
    dialogs/dialogdisplaydefinition.cpp \
    dialogs/dialogforcemultiplecoils.cpp \
    dialogs/dialogforcemultipleregisters.cpp \
    dialogs/dialogmaskwriteregiter.cpp \
    dialogs/dialogprotocolselections.cpp \
    dialogs/dialogsetuppresetdata.cpp \
    dialogs/dialogusermsg.cpp \
    dialogs/dialogwritecoilregister.cpp \
    dialogs/dialogwriteholdingregister.cpp \
    dialogs/dialogwriteholdingregisterbits.cpp \
    formmodsca.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusclient.cpp \
    qfixedsizedialog.cpp \
    qhexvalidator.cpp \
    recentfileactionlist.cpp \
    windowactionlist.cpp

HEADERS += \
    appsettings.h \
    connectiondetails.h \
    controls/booleancombobox.h \
    controls/bytelistlineedit.h \
    controls/connectioncombobox.h \
    controls/flowcontroltypecombobox.h \
    controls/numericlineedit.h \
    controls/paritytypecombobox.h \
    controls/statisticwidget.h \
    controls/numericcombobox.h \
    controls/outputwidget.h \
    controls/pointtypecombobox.h \
    dialogs/dialogabout.h \
    dialogs/dialogconnectiondetails.h \
    dialogs/dialogdisplaydefinition.h \
    dialogs/dialogforcemultiplecoils.h \
    dialogs/dialogforcemultipleregisters.h \
    dialogs/dialogmaskwriteregiter.h \
    dialogs/dialogprotocolselections.h \
    dialogs/dialogsetuppresetdata.h \
    dialogs/dialogusermsg.h \
    dialogs/dialogwritecoilregister.h \
    dialogs/dialogwriteholdingregister.h \
    dialogs/dialogwriteholdingregisterbits.h \
    displaydefinition.h \
    enums.h \
    floatutils.h \
    formmodsca.h \
    mainwindow.h \
    modbusclient.h \
    modbusexception.h \
    modbuslimits.h \
    modbuswriteparams.h \
    qfixedsizedialog.h \
    qhexvalidator.h \
    qrange.h \
    recentfileactionlist.h \
    windowactionlist.h

FORMS += \
    controls/outputwidget.ui \
    controls/statisticwidget.ui \
    dialogs/dialogabout.ui \
    dialogs/dialogconnectiondetails.ui \
    dialogs/dialogdisplaydefinition.ui \
    dialogs/dialogforcemultiplecoils.ui \
    dialogs/dialogforcemultipleregisters.ui \
    dialogs/dialogmaskwriteregiter.ui \
    dialogs/dialogprotocolselections.ui \
    dialogs/dialogsetuppresetdata.ui \
    dialogs/dialogusermsg.ui \
    dialogs/dialogwritecoilregister.ui \
    dialogs/dialogwriteholdingregister.ui \
    dialogs/dialogwriteholdingregisterbits.ui \
    formmodsca.ui \
    mainwindow.ui


RESOURCES += \
    resources.qrc
