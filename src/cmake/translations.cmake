set(TS_FILES
    translations/omodscan_ru.ts
    translations/omodscan_zh_CN.ts
    translations/omodscan_zh_TW.ts
)

target_sources(${PROJECT_NAME} PRIVATE ${TS_FILES})

if(Qt6_FOUND)
    qt_add_lupdate(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        OPTIONS -no-obsolete
    )
elseif(Qt5_FOUND AND EXISTS ${Qt5_LUPDATE_EXECUTABLE})
    add_custom_command(
        OUTPUT ${TS_FILES}
        COMMAND ${Qt5_LUPDATE_EXECUTABLE}
                ${SOURCES} ${HEADERS} ${UI_FILES}
                -no-obsolete
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
