# Findlibusb.cmake
# Find and install external libusb library

# Once done this will define
#
#  LIBUSB_FOUND         libusb present on system
#  LIBUSB_INCLUDE_DIR   the libusb include directory
#  LIBUSB_LIBRARY       the libraries needed to use libusb
#  LIBUSB_DEFINITIONS   compiler switches required for using libusb

include(FindPackageHandleStandardArgs)
include(FetchContent)

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")                       # FreeBSD; libusb is integrated into the system
    # libusb header file
    FIND_PATH(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS /usr/include
        )

    # libusb library
    set(LIBUSB_NAME usb)
    find_library(LIBUSB_LIBRARY
        NAMES ${LIBUSB_NAME}
        HINTS /usr /usr/local /opt
        )

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libusb DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    if (NOT LIBUSB_FOUND)
        message(FATAL_ERROR "Expected libusb library not found on your system! Verify your system integrity.")
    endif()

elseif (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")                   # OpenBSD; libusb is available from ports
    # libusb header file
    FIND_PATH(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS /usr/local/include
        PATH_SUFFIXES libusb-1.0
        )
    
    # libusb library
    set(LIBUSB_NAME usb-1.0)
    find_library(LIBUSB_LIBRARY
        NAMES ${LIBUSB_NAME}
        HINTS /usr/local
        )

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libusb DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    if (NOT LIBUSB_FOUND)
        message(FATAL_ERROR "No libusb-1.0 library found on your system! Install libusb-1.0 from ports or packages.")
    endif()

elseif (MSVC)      # Native Windows MSVC

    set(libusb_FIND_REQUIRED OFF) # Will either find it or download it, there's no missing it.
    set(LIBUSB_DEFINITIONS "-D_SSIZE_T_DEFINED" "-Dssize_t=int64_t") # fix for ill-defined ssize-t

    # libusb header file
    FIND_PATH(LIBUSB_INCLUDE_DIR
            NAMES libusb.h
            HINTS "C:/Program Files/libusb-1.0/include" "C:/Program Files (x86)/libusb-1.0/include"
            PATH_SUFFIXES "libusb-1.0"
    )

    # libusb library
    set(LIBUSB_NAME usb-1.0)
    find_library(LIBUSB_LIBRARY
            NAMES ${LIBUSB_NAME}
            HINTS "C:/Program Files/libusb-1.0" "C:/Program Files (x86)/libusb-1.0"
    )

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libusb DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    if (NOT LIBUSB_FOUND)
        message(STATUS "No libusb-1.0 not installed into your system. Downloading and building it from source")

        FetchContent_Declare(
                ${LIBUSB_NAME}
                GIT_REPOSITORY https://github.com/libusb/libusb-cmake
                GIT_TAG        v1.0.27-0
        )

        FetchContent_MakeAvailable(${LIBUSB_NAME})
        set(LIBUSB_FOUND ON)
        set(LIBUSB_INCLUDE_DIR "")
        set(LIBUSB_LIBRARY ${LIBUSB_NAME})
        mark_as_advanced(LIBUSB_FOUND LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    endif()
elseif (WIN32 OR (MINGW AND EXISTS "/etc/debian_version"))      # Windows OR cross-build with MinGW-toolchain on Debian
    # MinGW: 64-bit or 32-bit?
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "=== Building for Windows (x86-64) ===")
        set(ARCH 64)
    else ()
        message(STATUS "=== Building for Windows (i686) ===")
        set(ARCH 32)
    endif()

    if (NOT LIBUSB_FOUND)
        # Preparations for installing libusb library
        set(LIBUSB_WIN_VERSION 1.0.27) # set libusb version
        set(LIBUSB_WIN_ARCHIVE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libusb-${LIBUSB_WIN_VERSION}.7z)
        set(LIBUSB_WIN_OUTPUT_FOLDER ${CMAKE_SOURCE_DIR}/3rdparty/libusb-${LIBUSB_WIN_VERSION})

        # Get libusb package
        if (EXISTS ${LIBUSB_WIN_ARCHIVE_PATH})  # ... should the package be already there
            message(STATUS "libusb archive already in build folder")
        else ()                                 # ... download the package
            message(STATUS "downloading libusb ${LIBUSB_WIN_VERSION}")
            file(DOWNLOAD
                    https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-${LIBUSB_WIN_VERSION}/libusb-${LIBUSB_WIN_VERSION}.7z/download
                    ${LIBUSB_WIN_ARCHIVE_PATH} EXPECTED_MD5 c72153fc5a32f3b942427b0671897a1a
            )
        endif()

        file(MAKE_DIRECTORY ${LIBUSB_WIN_OUTPUT_FOLDER})

        # Extract libusb package with cmake
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xv ${LIBUSB_WIN_ARCHIVE_PATH}
                WORKING_DIRECTORY ${LIBUSB_WIN_OUTPUT_FOLDER}
        )

        # libusb header file
        FIND_PATH(LIBUSB_INCLUDE_DIR
                NAMES libusb.h
                HINTS ${LIBUSB_WIN_OUTPUT_FOLDER}/include
                PATH_SUFFIXES libusb-1.0
                NO_DEFAULT_PATH
                NO_CMAKE_FIND_ROOT_PATH
        )


        # libusb library (static)
        set(LIBUSB_NAME libusb-1.0)
        find_library(LIBUSB_LIBRARY
                NAMES ${LIBUSB_NAME}
                HINTS ${LIBUSB_WIN_OUTPUT_FOLDER}/MinGW${ARCH}/static
                NO_DEFAULT_PATH
                NO_CMAKE_FIND_ROOT_PATH
        )
    endif()

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libusb DEFAULT_MSG LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    message(STATUS "Missing libusb library has been installed")
else ()                                                         # all other OS (unix-based)
    # libusb header file
    FIND_PATH(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS /usr/include
        PATH_SUFFIXES libusb-1.0
        )
    
    # libusb library
    set(LIBUSB_NAME usb-1.0)
    find_library(LIBUSB_LIBRARY
        NAMES ${LIBUSB_NAME}
        HINTS /usr /usr/local
        )

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libusb DEFAULT_MSG LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)

    if (NOT LIBUSB_FOUND)
        message(FATAL_ERROR "libusb library not found on your system! Install libusb 1.0.x from your package repository.")
    endif()
endif()
