function(omodscan_configure_target_windows target_name)
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/omodscan.ico")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/omodscan.rc.in"
        "${PROJECT_BINARY_DIR}/omodscan.rc"
        @ONLY
    )

    target_sources(${target_name} PRIVATE "${PROJECT_BINARY_DIR}/omodscan.rc")
    set_target_properties(${target_name} PROPERTIES WIN32_EXECUTABLE ON)
endfunction()
