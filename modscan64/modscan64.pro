QT += core gui widgets network serialbus serialport
CONFIG += c++17
CONFIG += debug_and_release
CONFIG += debug_and_release_target

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += controls \

SOURCES += \
    appsettings.cpp \
    controls/connectioncombobox.cpp \
    controls/numericlineedit.cpp \
    controls/statisticwidget.cpp \
    dialogconnectiondetails.cpp \
    dialogdisplaydefinition.cpp \
    dialogprotocolselections.cpp \
    formmodsca.cpp \
    main.cpp \
    mainwindow.cpp \
    controls/numericcombobox.cpp \
    controls/outputwidget.cpp \
    controls/pointtypecombobox.cpp

HEADERS += \
    appsettings.h \
    connectiondetails.h \
    controls/connectioncombobox.h \
    controls/numericlineedit.h \
    controls/statisticwidget.h \
    dialogconnectiondetails.h \
    dialogdisplaydefinition.h \
    dialogprotocolselections.h \
    displaydefinition.h \
    enums.h \
    formmodsca.h \
    mainwindow.h \
    controls/numericcombobox.h \
    controls/outputwidget.h \
    controls/pointtypecombobox.h

FORMS += \
    controls/statisticwidget.ui \
    dialogconnectiondetails.ui \
    dialogdisplaydefinition.ui \
    dialogprotocolselections.ui \
    formmodsca.ui \
    mainwindow.ui \
    controls/outputwidget.ui

RESOURCES += \
    resources.qrc
