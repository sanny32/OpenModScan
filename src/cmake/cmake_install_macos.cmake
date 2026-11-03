if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/Applications" CACHE PATH "Install path prefix" FORCE)
endif()

if(Qt6_FOUND)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
else()
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif()
get_filename_component(QT_BINARY_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT_BINARY_DIR}" REQUIRED)
get_filename_component(QT_PLUGINS_DIR "${QT_BINARY_DIR}/../plugins" ABSOLUTE)
get_filename_component(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../translations" ABSOLUTE)

install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy_macos.cmake.in"
    "${PROJECT_BINARY_DIR}/deploy_macos.cmake"
    @ONLY
)
install(SCRIPT "${PROJECT_BINARY_DIR}/deploy_macos.cmake")
