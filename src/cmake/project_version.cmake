set(PRERELEASE_SUFFIX "" CACHE STRING "Pre-release suffix, e.g. beta1")

# Split version to major, minor, patch
string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

# Define current year
string(TIMESTAMP CURRENT_YEAR "%Y")

function(omodscan_apply_project_version_suffix)
    find_package(Git QUIET)
    if(NOT GIT_FOUND)
        message(STATUS "Git not found - branch detection is not possible")
    endif()

    if(DEFINED ENV{OMODSCAN_SOURCE_REF} OR DEFINED ENV{GITHUB_REF})
        set(SOURCE_REF "$ENV{GITHUB_REF}")
        if(DEFINED ENV{OMODSCAN_SOURCE_REF})
            set(SOURCE_REF "$ENV{OMODSCAN_SOURCE_REF}")
        endif()
        string(REGEX MATCH "refs/heads/(.*)" MATCH_RESULT "${SOURCE_REF}")
        if(MATCH_RESULT AND CMAKE_MATCH_1)
            set(GIT_BRANCH ${CMAKE_MATCH_1})
        endif()
    elseif(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    endif()

    if(DEFINED GIT_BRANCH AND "${GIT_BRANCH}" MATCHES "^(dev|release/.*)$")
        # A dev build carries the commit it was made from, so the version the
        # application reports identifies the exact source, as the packages do.
        set(DEV_SUFFIX "-dev")
        if(GIT_FOUND)
            execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
                OUTPUT_VARIABLE GIT_COMMIT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
        endif()
        # Container builds check out without git, fall back to the workflow sha.
        if(NOT GIT_COMMIT AND DEFINED ENV{GITHUB_SHA})
            string(SUBSTRING $ENV{GITHUB_SHA} 0 7 GIT_COMMIT)
        endif()
        if(GIT_COMMIT)
            string(APPEND DEV_SUFFIX ".g${GIT_COMMIT}")
        endif()
        set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}${DEV_SUFFIX} PARENT_SCOPE)
    elseif(PRERELEASE_SUFFIX)
        set(PROJECT_VERSION ${PROJECT_VERSION}-${PRERELEASE_SUFFIX} PARENT_SCOPE)
    endif()
endfunction()
