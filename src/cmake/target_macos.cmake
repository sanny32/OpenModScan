function(omodscan_configure_target_macos target_name)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(${target_name} PRIVATE ${IOKIT_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})

    set(MACOSX_COMPOSER_ICON_NAME "omodscan")
    set(MACOSX_COMPOSER_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/${MACOSX_COMPOSER_ICON_NAME}.icon")
    set(MACOSX_BUNDLE_ICON_FILE "${MACOSX_COMPOSER_ICON_NAME}")
    set(MACOSX_ASSET_ICON_PLIST_ENTRY "")

    if(EXISTS "${MACOSX_COMPOSER_ICON}")
        file(GLOB_RECURSE MACOSX_COMPOSER_ICON_FILES
            CONFIGURE_DEPENDS
            "${MACOSX_COMPOSER_ICON}/*"
        )

        find_program(ACTOOL_EXECUTABLE actool REQUIRED)

        set(MACOSX_ICON_DEPLOYMENT_TARGET "${CMAKE_OSX_DEPLOYMENT_TARGET}")
        if(NOT MACOSX_ICON_DEPLOYMENT_TARGET)
            set(MACOSX_ICON_DEPLOYMENT_TARGET "11.0")
        endif()

        set(MACOSX_ASSET_ICON_PLIST_ENTRY
"    <key>CFBundleIconName</key>
    <string>${MACOSX_COMPOSER_ICON_NAME}</string>")

        set(MACOSX_ICON_OUTPUT_DIR "$<TARGET_BUNDLE_CONTENT_DIR:${target_name}>/Resources")

        add_custom_target(${target_name}_macos_appicon ALL
            COMMAND "${CMAKE_COMMAND}" -E make_directory
                "${MACOSX_ICON_OUTPUT_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E remove_directory
                "${MACOSX_ICON_OUTPUT_DIR}/${MACOSX_COMPOSER_ICON_NAME}.icon"
            COMMAND "${ACTOOL_EXECUTABLE}"
                "${MACOSX_COMPOSER_ICON}"
                --app-icon "${MACOSX_COMPOSER_ICON_NAME}"
                --compile "${MACOSX_ICON_OUTPUT_DIR}"
                --output-partial-info-plist "${PROJECT_BINARY_DIR}/assetcatalog_generated_info.plist"
                --minimum-deployment-target "${MACOSX_ICON_DEPLOYMENT_TARGET}"
                --platform macosx
                --target-device mac
                --include-all-app-icons
                --enable-on-demand-resources NO
                --development-region en
            DEPENDS ${MACOSX_COMPOSER_ICON_FILES}
            COMMENT "Synchronizing macOS Icon Composer app icon assets"
        )
        add_dependencies(${target_name} ${target_name}_macos_appicon)
    else()
        message(FATAL_ERROR
            "No macOS app icon found. Expected ${MACOSX_COMPOSER_ICON}."
        )
    endif()

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
        MACOSX_BUNDLE_ICON_FILE "${MACOSX_BUNDLE_ICON_FILE}"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endfunction()
