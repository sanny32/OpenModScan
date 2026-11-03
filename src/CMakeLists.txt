cmake_minimum_required(VERSION 3.22)

project(omodscan
    VERSION 2.0.0
    DESCRIPTION "An Open Source Modbus Master (Client) Utility"
    LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PRODUCT_NAME "Open ModScan")

option(BUILD_TESTING "Build unit tests" ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/qt.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/project_version.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/generated_files.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ui_normalization.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/target_windows.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/target_macos.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/tests.cmake")

add_executable(${PROJECT_NAME})

omodscan_generate_files()
omodscan_apply_project_version_suffix()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_NAME="${PRODUCT_NAME}"
    APP_DESCRIPTION="${PROJECT_DESCRIPTION}"
    APP_VERSION="${PROJECT_VERSION}"
    BUILD_YEAR="${CURRENT_YEAR}"
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/controls
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/modbusmessages
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/sources.cmake")
omodscan_configure_ui_enum_normalization(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE resources.qrc ${HEADERS} ${SOURCES} ${UI_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Network Qt::PrintSupport Qt::SerialBus Qt::SerialPort)
omodscan_configure_qt_target(${PROJECT_NAME})

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/translations.cmake")

if(WIN32)
    omodscan_configure_target_windows(${PROJECT_NAME})
elseif(APPLE)
    omodscan_configure_target_macos(${PROJECT_NAME})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_install_macos.cmake")
elseif(LINUX)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_install_linux.cmake")
endif()

if(BUILD_TESTING)
    enable_testing()
    omodscan_configure_tests()
endif()
