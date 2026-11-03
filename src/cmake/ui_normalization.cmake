function(omodscan_configure_ui_enum_normalization target_name)
    set(UI_FILE_LIST "${CMAKE_CURRENT_BINARY_DIR}/ui_enum_files.txt")
    set(UI_FILES_ABSOLUTE)
    file(WRITE "${UI_FILE_LIST}" "")
    foreach(UI_FILE IN LISTS UI_FILES)
        set(UI_FILE_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/${UI_FILE}")
        list(APPEND UI_FILES_ABSOLUTE "${UI_FILE_ABSOLUTE}")
        file(APPEND "${UI_FILE_LIST}" "${UI_FILE_ABSOLUTE}\n")
    endforeach()

    add_custom_target(normalize_ui_enums ALL
        COMMAND ${CMAKE_COMMAND}
                "-DUI_FILE_LIST=${UI_FILE_LIST}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/normalize_ui_enums.cmake"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/normalize_ui_enums.cmake"
            ${UI_FILES_ABSOLUTE}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Normalizing Qt Designer UI enum names for Qt5 compatibility..."
        VERBATIM
    )

    add_dependencies(${target_name} normalize_ui_enums)
    set_property(TARGET ${target_name} APPEND PROPERTY AUTOGEN_TARGET_DEPENDS normalize_ui_enums)
endfunction()
