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

omodscan_generate_files()
omodscan_apply_project_version_suffix()

# Preserve the output paths used by build scripts and package workflows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

if(BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(app)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
