QT += core gui widgets network serialbus serialport
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appsettings.cpp \
    dialogdisplaydefinition.cpp \
    formmodsca.cpp \
    main.cpp \
    mainwindow.cpp \
    numberlineedit.cpp \
    outputwidget.cpp \
    pointtypecombobox.cpp

HEADERS += \
    appsettings.h \
    dialogdisplaydefinition.h \
    displaydefinition.h \
    enums.h \
    formmodsca.h \
    mainwindow.h \
    numberlineedit.h \
    outputwidget.h \
    pointtypecombobox.h

FORMS += \
    dialogdisplaydefinition.ui \
    formmodsca.ui \
    mainwindow.ui \
    outputwidget.ui

RESOURCES += \
    resources.qrc
