QT += core gui widgets network serialbus serialport
CONFIG += c++17
CONFIG += debug_and_release
CONFIG += debug_and_release_target

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:RC_ICONS += res/modscan64.ico

INCLUDEPATH += controls \

SOURCES += \
    appsettings.cpp \
    controls/booleancombobox.cpp \
    controls/connectioncombobox.cpp \
    controls/flowcontroltypecombobox.cpp \
    controls/numericlineedit.cpp \
    controls/paritytypecombobox.cpp \
    controls/statisticwidget.cpp \
    controls/numericcombobox.cpp \
    controls/outputwidget.cpp \
    controls/pointtypecombobox.cpp \
    dialogconnectiondetails.cpp \
    dialogdisplaydefinition.cpp \
    dialogforcemultiplecoils.cpp \
    dialogforcemultipleregisters.cpp \
    dialogmaskwriteregiter.cpp \
    dialogprotocolselections.cpp \
    dialogsetuppresetdata.cpp \
    dialogwritecoilregister.cpp \
    dialogwriteholdingregister.cpp \
    dialogwriteholdingregisterbits.cpp \
    formmodsca.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusclient.cpp \
    qfixedsizedialog.cpp \
    qhexvalidator.cpp

HEADERS += \
    appsettings.h \
    connectiondetails.h \
    controls/booleancombobox.h \
    controls/connectioncombobox.h \
    controls/flowcontroltypecombobox.h \
    controls/numericlineedit.h \
    controls/paritytypecombobox.h \
    controls/statisticwidget.h \
    controls/numericcombobox.h \
    controls/outputwidget.h \
    controls/pointtypecombobox.h \
    dialogconnectiondetails.h \
    dialogdisplaydefinition.h \
    dialogforcemultiplecoils.h \
    dialogforcemultipleregisters.h \
    dialogmaskwriteregiter.h \
    dialogprotocolselections.h \
    dialogsetuppresetdata.h \
    dialogwritecoilregister.h \
    dialogwriteholdingregister.h \
    dialogwriteholdingregisterbits.h \
    displaydefinition.h \
    enums.h \
    formmodsca.h \
    mainwindow.h \
    modbusclient.h \
    modbusexception.h \
    modbuslimits.h \
    modbuswriteparams.h \
    qfixedsizedialog.h \
    qhexvalidator.h \
    qrange.h

FORMS += \
    controls/outputwidget.ui \
    controls/statisticwidget.ui \
    dialogconnectiondetails.ui \
    dialogdisplaydefinition.ui \
    dialogforcemultiplecoils.ui \
    dialogforcemultipleregisters.ui \
    dialogmaskwriteregiter.ui \
    dialogprotocolselections.ui \
    dialogsetuppresetdata.ui \
    dialogwritecoilregister.ui \
    dialogwriteholdingregister.ui \
    dialogwriteholdingregisterbits.ui \
    formmodsca.ui \
    mainwindow.ui


RESOURCES += \
    resources.qrc
