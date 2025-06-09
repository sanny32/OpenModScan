cmake_minimum_required(VERSION 3.25)

project(omodscan
    VERSION 1.9.1
    DESCRIPTION "An Open Source Modbus Master (Client) Utility"
    LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 or fallback to Qt5
find_package(Qt6 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort Core5Compat LinguistTools QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort LinguistTools QUIET)
endif()

# Define target
add_executable(omodscan)

# Define compile definitions
add_compile_definitions(APP_NAME="Open ModScan")
add_compile_definitions(APP_DESCRIPTION="${PROJECT_DESCRIPTION}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

# Include directories
target_include_directories(omodscan PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/controls
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/modbusmessages
)

# Source files
set(SOURCES
    ansimenu.cpp
    controls/addressbasecombobox.cpp
    controls/booleancombobox.cpp
    controls/bytelisttextedit.cpp
    controls/byteordercombobox.cpp
    controls/clickablelabel.cpp
    controls/connectioncombobox.cpp
    controls/customlineedit.cpp
    controls/flowcontroltypecombobox.cpp
    controls/formattedspinbox.cpp
    controls/functioncodecombobox.cpp
    controls/ipaddresslineedit.cpp
    controls/mainstatusbar.cpp
    controls/modbuslogwidget.cpp
    controls/modbusmessagewidget.cpp
    controls/numericlineedit.cpp
    controls/paritytypecombobox.cpp
    controls/simulationmodecombobox.cpp
    controls/statisticwidget.cpp
    controls/numericcombobox.cpp
    controls/outputwidget.cpp
    controls/pointtypecombobox.cpp
    datasimulator.cpp
    dialogs/dialogmsgparser.cpp
    dialogs/dialogabout.cpp
    dialogs/dialogaddressscan.cpp
    dialogs/dialogautosimulation.cpp
    dialogs/dialogautostart.cpp
    dialogs/dialogcoilsimulation.cpp
    dialogs/dialogconnectiondetails.cpp
    dialogs/dialogdisplaydefinition.cpp
    dialogs/dialogforcemultiplecoils.cpp
    dialogs/dialogforcemultipleregisters.cpp
    dialogs/dialogmaskwriteregiter.cpp
    dialogs/dialogmodbusscanner.cpp
    dialogs/dialogprintsettings.cpp
    dialogs/dialogprotocolselections.cpp
    dialogs/dialogsetuppresetdata.cpp
    dialogs/dialogusermsg.cpp
    dialogs/dialogwindowsmanager.cpp
    dialogs/dialogwritecoilregister.cpp
    dialogs/dialogwriteholdingregister.cpp
    dialogs/dialogwriteholdingregisterbits.cpp
    formmodsca.cpp
    htmldelegate.cpp
    main.cpp
    mainwindow.cpp
    modbusclient.cpp
    modbusdataunit.cpp
    modbusmessages/modbusmessage.cpp
    modbusrtuscanner.cpp
    modbusscanner.cpp
    modbustcpscanner.cpp
    qfixedsizedialog.cpp
    qhexvalidator.cpp
    qint64validator.cpp
    quintvalidator.cpp
    recentfileactionlist.cpp
    windowactionlist.cpp
)

# Header files
set(HEADERS
    ansimenu.h
    ansiutils.h
    byteorderutils.h
    connectiondetails.h
    controls/addressbasecombobox.h
    controls/booleancombobox.h
    controls/bytelisttextedit.h
    controls/byteordercombobox.h
    controls/clickablelabel.h
    controls/connectioncombobox.h
    controls/customlineedit.h
    controls/flowcontroltypecombobox.h
    controls/formattedspinbox.h
    controls/functioncodecombobox.h
    controls/ipaddresslineedit.h
    controls/mainstatusbar.h
    controls/modbuslogwidget.h
    controls/modbusmessagewidget.h
    controls/numericlineedit.h
    controls/paritytypecombobox.h
    controls/simulationmodecombobox.h
    controls/statisticwidget.h
    controls/numericcombobox.h
    controls/outputwidget.h
    controls/pointtypecombobox.h
    datasimulator.h
    dialogs/dialogmsgparser.h
    dialogs/dialogabout.h
    dialogs/dialogaddressscan.h
    dialogs/dialogautosimulation.h
    dialogs/dialogautostart.h
    dialogs/dialogcoilsimulation.h
    dialogs/dialogconnectiondetails.h
    dialogs/dialogdisplaydefinition.h
    dialogs/dialogforcemultiplecoils.h
    dialogs/dialogforcemultipleregisters.h
    dialogs/dialogmaskwriteregiter.h
    dialogs/dialogmodbusscanner.h
    dialogs/dialogprintsettings.h
    dialogs/dialogprotocolselections.h
    dialogs/dialogsetuppresetdata.h
    dialogs/dialogusermsg.h
    dialogs/dialogwindowsmanager.h
    dialogs/dialogwritecoilregister.h
    dialogs/dialogwriteholdingregister.h
    dialogs/dialogwriteholdingregisterbits.h
    displaydefinition.h
    enums.h
    formatutils.h
    formmodsca.h
    htmldelegate.h
    mainwindow.h
    modbusclient.h
    modbusdataunit.h
    modbusexception.h
    modbusfunction.h
    modbusmessages/diagnostics.h
    modbusmessages/getcommeventcounter.h
    modbusmessages/getcommeventlog.h
    modbusmessages/maskwriteregister.h
    modbusmessages/modbusmessage.h
    modbusmessages/modbusmessages.h
    modbusmessages/readcoils.h
    modbuslimits.h
    modbusmessages/readdiscreteinputs.h
    modbusmessages/readexceptionstatus.h
    modbusmessages/readfifoqueue.h
    modbusmessages/readfilerecord.h
    modbusmessages/readholdingregisters.h
    modbusmessages/readinputregisters.h
    modbusmessages/readwritemultipleregisters.h
    modbusmessages/reportserverid.h
    modbusmessages/writefilerecord.h
    modbusmessages/writemultiplecoils.h
    modbusmessages/writemultipleregisters.h
    modbusmessages/writesinglecoil.h
    modbusmessages/writesingleregister.h
    modbusrtuscanner.h
    modbusscanner.h
    modbussimulationparams.h
    modbustcpscanner.h
    modbuswriteparams.h
    numericutils.h
    qfixedsizedialog.h
    qhexvalidator.h
    qint64validator.h
    qmodbusadu.h
    qmodbusadurtu.h
    qmodbusadutcp.h
    qrange.h
    quintvalidator.h
    recentfileactionlist.h
    serialportutils.h
    windowactionlist.h
)

# UI Forms
set(UI_FILES
    controls/outputwidget.ui
    controls/statisticwidget.ui
    dialogs/dialogmsgparser.ui
    dialogs/dialogabout.ui
    dialogs/dialogaddressscan.ui
    dialogs/dialogautosimulation.ui
    dialogs/dialogautostart.ui
    dialogs/dialogcoilsimulation.ui
    dialogs/dialogconnectiondetails.ui
    dialogs/dialogdisplaydefinition.ui
    dialogs/dialogforcemultiplecoils.ui
    dialogs/dialogforcemultipleregisters.ui
    dialogs/dialogmaskwriteregiter.ui
    dialogs/dialogmodbusscanner.ui
    dialogs/dialogprintsettings.ui
    dialogs/dialogprotocolselections.ui
    dialogs/dialogsetuppresetdata.ui
    dialogs/dialogusermsg.ui
    dialogs/dialogwindowsmanager.ui
    dialogs/dialogwritecoilregister.ui
    dialogs/dialogwriteholdingregister.ui
    dialogs/dialogwriteholdingregisterbits.ui
    formmodsca.ui
    mainwindow.ui
)

set(TS_FILES
    translations/omodscan_ru.ts
    translations/omodscan_cn.ts
    translations/omodscan_zh.ts
)

qt_add_lupdate(omodscan
    TS_FILES ${TS_FILES}
    SOURCES ${SOURCES} ${HEADERS} ${UI_FILES}
    OUTPUT_DIR translations
)

# Link libraries
target_sources(omodscan PRIVATE resources.qrc ${HEADERS} ${SOURCES} ${UI_FILES} ${TS_FILES})
target_link_libraries(omodscan PRIVATE Qt::Widgets Qt::Network Qt::PrintSupport Qt::SerialBus Qt::SerialPort)

if(Qt6_FOUND)
    target_link_libraries(omodscan PRIVATE Qt::Core5Compat)
endif()

if(WIN32)
    target_sources(omodscan PRIVATE res/omodscan.ico)
    set_target_properties(omodscan PROPERTIES WIN32_EXECUTABLE ON)
endif()

