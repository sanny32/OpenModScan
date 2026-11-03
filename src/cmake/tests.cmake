function(omodscan_configure_tests)
    if(Qt6_FOUND)
        find_package(Qt6 COMPONENTS Test REQUIRED)
        set(QT_CORE_TARGET Qt6::Core)
    else()
        find_package(Qt5 COMPONENTS Test REQUIRED)
        set(QT_CORE_TARGET Qt5::Core)
    endif()

    set(TEST_TARGETS tst_modbustcpclient tst_modbusrtutraffic tst_mbmap tst_core tst_datasimulator)
    foreach(TEST_TARGET IN LISTS TEST_TARGETS)
        add_executable(${TEST_TARGET} ${TEST_TARGET}.cpp)
        target_link_libraries(${TEST_TARGET} PRIVATE omodscan_core Qt::Test)
        add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
    endforeach()

    add_custom_target(check-build DEPENDS ${TEST_TARGETS})
    set_tests_properties(${TEST_TARGETS} PROPERTIES TIMEOUT 60)

    if(WIN32)
        # Qt5's Qt::Core is an INTERFACE wrapper, so use the versioned target.
        set_tests_properties(${TEST_TARGETS} PROPERTIES
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:${QT_CORE_TARGET}>")
    endif()
endfunction()
