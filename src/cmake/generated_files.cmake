function(omodscan_generate_files)
    configure_file(
        ${PROJECT_SOURCE_DIR}/app/res/license.txt.in
        ${PROJECT_SOURCE_DIR}/app/res/license.txt
        @ONLY
    )

    configure_file(
        ${PROJECT_SOURCE_DIR}/../README.md.in
        ${PROJECT_SOURCE_DIR}/../README.md
        @ONLY
    )
endfunction()
