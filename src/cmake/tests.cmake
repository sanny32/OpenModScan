if(Qt6_FOUND)
    find_package(Qt6 COMPONENTS Test QUIET)
    set(QT_TEST_FOUND ${Qt6Test_FOUND})
    set(QT_CORE_TARGET Qt6::Core)
else()
    find_package(Qt5 COMPONENTS Test QUIET)
    set(QT_TEST_FOUND ${Qt5Test_FOUND})
    set(QT_CORE_TARGET Qt5::Core)
endif()

if(NOT QT_TEST_FOUND)
    message(STATUS "Qt Test not found - tests are disabled")
    return()
endif()

enable_testing()

add_executable(tst_modbustcpclient
    tests/tst_modbustcpclient.cpp
    modbusclientprivate.cpp
    modbusclientprivate.h
    modbusdevice.h
    modbusmessages/modbusmessage.cpp
    modbusreply.cpp
    modbusreply.h
    modbustcpclient.cpp
    modbustcpclient.h
)

target_include_directories(tst_modbustcpclient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/modbusmessages
)

target_link_libraries(tst_modbustcpclient PRIVATE Qt::Test Qt::Network Qt::SerialBus)
if(Qt6_FOUND)
    target_link_libraries(tst_modbustcpclient PRIVATE Qt::Core5Compat)
endif()

add_test(NAME tst_modbustcpclient COMMAND tst_modbustcpclient)

add_executable(tst_modbusrtutraffic
    tests/tst_modbusrtutraffic.cpp
    modbusclientprivate.cpp
    modbusclientprivate.h
    modbusdevice.h
    modbusmessages/modbusmessage.cpp
    modbusreply.cpp
    modbusreply.h
    modbusrtuclient.cpp
    modbusrtuclient.h
    modbusrtutcpclient.cpp
    modbusrtutcpclient.h
    qobjecttimer.h
)

target_include_directories(tst_modbusrtutraffic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/modbusmessages
)
target_link_libraries(tst_modbusrtutraffic PRIVATE Qt::Test Qt::Network Qt::SerialBus Qt::SerialPort)
if(Qt6_FOUND)
    target_link_libraries(tst_modbusrtutraffic PRIVATE Qt::Core5Compat)
endif()

add_test(NAME tst_modbusrtutraffic COMMAND tst_modbusrtutraffic)

add_executable(tst_mbmap
    tests/tst_mbmap.cpp
    mbmap.cpp
    mbmap.h
)

target_include_directories(tst_mbmap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tst_mbmap PRIVATE Qt::Test Qt::SerialBus)

add_test(NAME tst_mbmap COMMAND tst_mbmap)

if(WIN32)
    # Qt DLLs are not deployed next to test executables; versioned target because Qt5's Qt::Core is an INTERFACE wrapper.
    set_tests_properties(tst_modbustcpclient tst_modbusrtutraffic tst_mbmap PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:${QT_CORE_TARGET}>")
endif()
