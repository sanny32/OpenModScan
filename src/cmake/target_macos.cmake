function(omodscan_configure_target_macos target_name)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(${target_name} PRIVATE ${IOKIT_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})

    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/omodscan.icns")
    set_source_files_properties("${APP_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(${target_name} PRIVATE "${APP_ICON}")

    set(MACOSX_MIN_VERSION_PLIST_ENTRY "")
    if(CMAKE_OSX_DEPLOYMENT_TARGET)
        set(MACOSX_MIN_VERSION_PLIST_ENTRY
            "    <key>LSMinimumSystemVersion</key>\n    <string>${CMAKE_OSX_DEPLOYMENT_TARGET}</string>")
    endif()

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/omodscan.plist.in"
        "${PROJECT_BINARY_DIR}/Info.plist"
        @ONLY
    )

    set_target_properties(${target_name} PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_BINARY_DIR}/Info.plist"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endfunction()
