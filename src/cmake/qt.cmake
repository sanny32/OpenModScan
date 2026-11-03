option(USE_QT5 "Force Qt5 usage" OFF)
option(USE_QT6 "Force Qt6 usage" OFF)

set(QT_COMMON_COMPONENTS Core Gui Widgets Network PrintSupport SerialBus SerialPort LinguistTools)

if(USE_QT5)
    find_package(Qt5 COMPONENTS ${QT_COMMON_COMPONENTS} REQUIRED)
elseif(USE_QT6)
    find_package(Qt6 COMPONENTS ${QT_COMMON_COMPONENTS} Core5Compat REQUIRED)
else()
    # fallback: Qt6 preferred
    find_package(Qt6 COMPONENTS ${QT_COMMON_COMPONENTS} Core5Compat QUIET)
    if (NOT Qt6_FOUND)
        find_package(Qt5 COMPONENTS ${QT_COMMON_COMPONENTS} QUIET)
    endif()
endif()

if (NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(FATAL_ERROR "One or more Qt development packages not found. Please install Qt5 or Qt6 development packages.")
endif()

function(omodscan_configure_qt_target target_name)
    if(Qt6_FOUND)
        target_link_libraries(${target_name} PRIVATE Qt::Core5Compat)
    endif()
endfunction()
