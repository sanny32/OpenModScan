cmake_minimum_required(VERSION 3.16)

project(omodscan
    VERSION 1.13.0
    DESCRIPTION "An Open Source Modbus Master (Client) Utility"
    LANGUAGES CXX)

option(USE_QT5 "Force Qt5 usage" OFF)
option(USE_QT6 "Force Qt6 usage" OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PRODUCT_NAME "Open ModScan")
set(EXECUTABLE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")

# Split version to major, minor, patch
string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

if(USE_QT5)
    find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort LinguistTools REQUIRED)
elseif(USE_QT6)
    find_package(Qt6 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort Core5Compat LinguistTools REQUIRED)
else()
    # fallback: Qt6 preferred
    find_package(Qt6 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort Core5Compat LinguistTools QUIET)
    if (NOT Qt6_FOUND)
        find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort LinguistTools QUIET)
    endif()
endif()

if (NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(FATAL_ERROR "One or more Qt development packages not found. Please install Qt5 or Qt6 development packages.")
endif()


# Define target
add_executable(${PROJECT_NAME})

if(DEFINED ENV{GITHUB_REF})
    string(REGEX MATCH "refs/heads/(.*)" MATCH_RESULT $ENV{GITHUB_REF})
    if(MATCH_RESULT AND CMAKE_MATCH_1)
        set(GIT_BRANCH ${CMAKE_MATCH_1})
    endif()
else()
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../.git
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    else()
        message(STATUS "Git not found - branch detection is not possible")
    endif()
endif()

if(${GIT_BRANCH} MATCHES dev)
    set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}-dev)
endif()

# Define compile definitions
add_compile_definitions(APP_NAME="${PRODUCT_NAME}")
add_compile_definitions(APP_DESCRIPTION="${PROJECT_DESCRIPTION}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/controls
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/modbusmessages
)

# Source files
set(SOURCES
    ansimenu.cpp
    controls/aboutdatawidget.cpp
    controls/addressbasecombobox.cpp
    controls/booleancombobox.cpp
    controls/bytelisttextedit.cpp
    controls/byteordercombobox.cpp
    controls/clickablelabel.cpp
    controls/serialportcombobox.cpp
    controls/customlineedit.cpp
    controls/flowcontroltypecombobox.cpp
    controls/formattedspinbox.cpp
    controls/functioncodecombobox.cpp
    controls/historycombobox.cpp
    controls/hostscombobox.cpp
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
    modbusclientprivate.cpp
    modbustcpclient.cpp
    modbusrtuclient.cpp
    modbusrtutcpclient.cpp
    modbusdataunit.cpp
    modbusmessages/modbusmessage.cpp
    modbusreply.cpp
    modbusrtuscanner.cpp
    modbusscanner.cpp
    modbustcpscanner.cpp
    qfixedsizedialog.cpp
    qhexvalidator.cpp
    qint64validator.cpp
    quintvalidator.cpp
    recentfileactionlist.cpp
    tscrollarea.cpp
    windowactionlist.cpp
)

# Header files
set(HEADERS
    ansimenu.h
    ansiutils.h
    byteorderutils.h
    connectiondetails.h
    controls/aboutdatawidget.h
    controls/addressbasecombobox.h
    controls/booleancombobox.h
    controls/bytelisttextedit.h
    controls/byteordercombobox.h
    controls/clickablelabel.h
    controls/serialportcombobox.h
    controls/customlineedit.h
    controls/flowcontroltypecombobox.h
    controls/formattedspinbox.h
    controls/functioncodecombobox.h
    controls/historycombobox.h
    controls/hostscombobox.h
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
    fontutils.h
    formatutils.h
    formmodsca.h
    htmldelegate.h
    mainwindow.h
    modbusclient.h
    modbusclientprivate.h
    modbustcpclient.h
    modbusrtutcpclient.h
    modbusreply.h
    modbusrtuclient.h
    modbusdataunit.h
    modbusdevice.h
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
    tscrollarea.h
    waitcursor.h
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
    translations/omodscan_zh_CN.ts
    translations/omodscan_zh_TW.ts
)

# Link libraries
target_sources(${PROJECT_NAME} PRIVATE resources.qrc ${HEADERS} ${SOURCES} ${UI_FILES} ${TS_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Network Qt::PrintSupport Qt::SerialBus Qt::SerialPort)

if(Qt6_FOUND)
    qt_add_lupdate(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        SOURCES ${SOURCES} ${HEADERS} ${UI_FILES}
    )
    target_link_libraries(omodscan PRIVATE Qt::Core5Compat)
elseif(Qt5_FOUND AND EXISTS ${Qt5_LUPDATE_EXECUTABLE})
    add_custom_command(
        OUTPUT ${TS_FILES}
        COMMAND ${Qt5_LUPDATE_EXECUTABLE}
                ${SOURCES} ${HEADERS} ${UI_FILES}
                -ts ${TS_FILES}
        DEPENDS ${SOURCES} ${HEADERS} ${UI_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Updating translation files..."
    )
    add_custom_target(update_translations ALL DEPENDS ${TS_FILES})
else()
    message(STATUS "lupdate not available - skipping translation updates")
    add_custom_target(update_translations)
endif()

if(TARGET update_translations)
    add_dependencies(${PROJECT_NAME} update_translations)
endif()

if(WIN32)
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/omodscan.ico")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/omodscan.rc.in"
        "${CMAKE_BINARY_DIR}/omodscan.rc"
        @ONLY
    )

    target_sources(${PROJECT_NAME} PRIVATE ${ICON_PATH} "${CMAKE_BINARY_DIR}/omodscan.rc")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

if(LINUX)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/omodscan.desktop.in"
        "${CMAKE_CURRENT_BINARY_DIR}/omodscan.desktop"
        @ONLY
    )

    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    install(FILES res/omodscan16.png DESTINATION share/icons/hicolor/16x16/apps RENAME omodscan.png)
    install(FILES res/omodscan32.png DESTINATION share/icons/hicolor/32x32/apps RENAME omodscan.png)
    install(FILES res/omodscan64.png DESTINATION share/icons/hicolor/64x64/apps RENAME omodscan.png)
    install(FILES res/omodscan128.png DESTINATION share/icons/hicolor/128x128/apps RENAME omodscan.png)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/omodscan.desktop" DESTINATION share/applications)

    install(CODE "execute_process(COMMAND update-desktop-database -q ${CMAKE_INSTALL_PREFIX}/share/applications)")
    install(CODE "execute_process(COMMAND gtk-update-icon-cache -q -t -f ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)")
endif()
