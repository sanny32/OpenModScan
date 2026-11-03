add_executable(${PROJECT_NAME}
    main.cpp
    cmdlineparser.cpp
    cmdlineparser.h
    resources.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE omodscan_ui)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_NAME="${PRODUCT_NAME}"
    APP_DESCRIPTION="${PROJECT_DESCRIPTION}"
    APP_VERSION="${PROJECT_VERSION}"
    BUILD_YEAR="${CURRENT_YEAR}"
)

include("${PROJECT_SOURCE_DIR}/cmake/translations.cmake")

if(WIN32)
    omodscan_configure_target_windows(${PROJECT_NAME})
elseif(APPLE)
    omodscan_configure_target_macos(${PROJECT_NAME})
    include("${PROJECT_SOURCE_DIR}/cmake/cmake_install_macos.cmake")
elseif(LINUX)
    include("${PROJECT_SOURCE_DIR}/cmake/cmake_install_linux.cmake")
endif()
