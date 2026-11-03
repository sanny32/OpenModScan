function(omodscan_generate_files)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt.in
        ${CMAKE_CURRENT_SOURCE_DIR}/res/license.txt
        @ONLY
    )

    configure_file(
        ${PROJECT_SOURCE_DIR}/../README.md.in
        ${PROJECT_SOURCE_DIR}/../README.md
        @ONLY
    )
endfunction()
