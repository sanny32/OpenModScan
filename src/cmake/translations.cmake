set(TRANSLATION_SOURCES)
foreach(SOURCE_TARGET IN ITEMS omodscan_core omodscan_ui ${PROJECT_NAME})
    get_target_property(TARGET_SOURCES ${SOURCE_TARGET} SOURCES)
    get_target_property(TARGET_SOURCE_DIR ${SOURCE_TARGET} SOURCE_DIR)
    foreach(SOURCE IN LISTS TARGET_SOURCES)
        if(SOURCE MATCHES "\\.(cpp|h|ui)$")
            list(APPEND TRANSLATION_SOURCES "${TARGET_SOURCE_DIR}/${SOURCE}")
        endif()
    endforeach()
endforeach()

set(TS_FILES
    translations/omodscan_it.ts
    translations/omodscan_ru.ts
    translations/omodscan_zh_CN.ts
    translations/omodscan_zh_TW.ts
)

target_sources(${PROJECT_NAME} PRIVATE ${TS_FILES})

if(Qt6_FOUND)
    qt_add_lupdate(${PROJECT_NAME}
        SOURCES ${TRANSLATION_SOURCES}
        TS_FILES ${TS_FILES}
        OPTIONS -no-obsolete -locations none
    )
    if(NOT TARGET update_translations)
        add_custom_target(update_translations)
    endif()
    add_dependencies(update_translations ${PROJECT_NAME}_lupdate)

    qt_add_lrelease(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        QM_FILES_OUTPUT_VARIABLE QM_FILES
    )
elseif(Qt5_FOUND)
    add_custom_target(update_translations
        COMMAND ${Qt5_LUPDATE_EXECUTABLE}
                ${TRANSLATION_SOURCES}
                -no-obsolete -locations none
                -ts ${TS_FILES}
        DEPENDS ${TRANSLATION_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Updating translation files..."
        VERBATIM
    )

    qt5_add_translation(QM_FILES ${TS_FILES})
endif()

# Compile translations in the build tree and preserve the resource paths used
# by MainWindow. Updating the source catalogs is an explicit operation only.
set(TRANSLATIONS_QRC "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc")
set(TRANSLATIONS_QRC_CONTENT "<RCC>\n    <qresource prefix=\"/translations\">\n")
foreach(QM_FILE IN LISTS QM_FILES)
    get_filename_component(QM_NAME "${QM_FILE}" NAME)
    string(APPEND TRANSLATIONS_QRC_CONTENT "        <file alias=\"${QM_NAME}\">${QM_FILE}</file>\n")
endforeach()
string(APPEND TRANSLATIONS_QRC_CONTENT "    </qresource>\n</RCC>\n")
file(CONFIGURE OUTPUT "${TRANSLATIONS_QRC}" CONTENT "${TRANSLATIONS_QRC_CONTENT}" @ONLY)
target_sources(${PROJECT_NAME} PRIVATE "${TRANSLATIONS_QRC}" ${QM_FILES})
