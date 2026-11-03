add_library(omodscan_core STATIC
    ansiutils.h
    byteorderutils.h
    datasimulator.cpp
    datasimulator.h
    displaydefinition.h
    enums.h
    formatutils.h
    mbmap.cpp
    mbmap.h
    modbus/connectiondetails.h
    modbus/messages/diagnostics.h
    modbus/messages/getcommeventcounter.h
    modbus/messages/getcommeventlog.h
    modbus/messages/maskwriteregister.h
    modbus/messages/modbusmessage.cpp
    modbus/messages/modbusmessage.h
    modbus/messages/modbusmessages.h
    modbus/messages/readcoils.h
    modbus/messages/readdiscreteinputs.h
    modbus/messages/readexceptionstatus.h
    modbus/messages/readfifoqueue.h
    modbus/messages/readfilerecord.h
    modbus/messages/readholdingregisters.h
    modbus/messages/readinputregisters.h
    modbus/messages/readwritemultipleregisters.h
    modbus/messages/reportserverid.h
    modbus/messages/writefilerecord.h
    modbus/messages/writemultiplecoils.h
    modbus/messages/writemultipleregisters.h
    modbus/messages/writesinglecoil.h
    modbus/messages/writesingleregister.h
    modbus/modbusclient.cpp
    modbus/modbusclient.h
    modbus/modbusclientprivate.cpp
    modbus/modbusclientprivate.h
    modbus/modbusdataunit.cpp
    modbus/modbusdataunit.h
    modbus/modbusdevice.h
    modbus/modbusexception.h
    modbus/modbusfunction.h
    modbus/modbuslimits.h
    modbus/modbusreply.cpp
    modbus/modbusreply.h
    modbus/modbusrtuclient.cpp
    modbus/modbusrtuclient.h
    modbus/modbusrtuscanner.cpp
    modbus/modbusrtuscanner.h
    modbus/modbusrtutcpclient.cpp
    modbus/modbusrtutcpclient.h
    modbus/modbusrtutcpscanner.h
    modbus/modbusscanner.cpp
    modbus/modbusscanner.h
    modbus/modbussimulationparams.h
    modbus/modbustcpclient.cpp
    modbus/modbustcpclient.h
    modbus/modbustcpscanner.cpp
    modbus/modbustcpscanner.h
    modbus/modbuswriteparams.h
    modbus/qmodbusadu.h
    modbus/qmodbusadurtu.h
    modbus/qmodbusadutcp.h
    modbus/qmodbusserialadu.h
    modbus/serialportutils.h
    numericutils.h
    qobjecttimer.h
    qrange.h
)

target_include_directories(omodscan_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/messages
)

target_link_libraries(omodscan_core PUBLIC Qt::Core Qt::Network Qt::SerialBus Qt::SerialPort)
if(Qt6_FOUND)
    target_link_libraries(omodscan_core PUBLIC Qt::Core5Compat)
endif()

if(APPLE)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(omodscan_core PUBLIC ${IOKIT_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})
endif()
