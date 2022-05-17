#
# Find the TwinCAT ADS library and wrap it with a CMake target
#

if (NOT TWINCAT_DIR)
    set(_twincat3dir "C:/TwinCAT/3.1")
    if (DEFINED ENV{TWINCAT3DIR})
        set(_twincat3dir "$ENV{TWINCAT3DIR}")
    endif ()

    get_filename_component(TWINCAT_DIR ${_twincat3dir} DIRECTORY)
endif ()

# Find include directory
find_path(ADS_INCLUDE_DIR
        NAMES "TcAdsAPI.h"
        DOC "ADS include directory"
        PATH_SUFFIXES "Include"
        PATHS "${TWINCAT_DIR}/AdsApi/TcAdsDll")
mark_as_advanced(ADS_INCLUDE_DIR)

# Find library
if (NOT ADS_LIBRARY)

    find_library(ADS_LIBRARY
            NAMES "TcAdsDll"
            DOC "ADS library (static)"
            PATH_SUFFIXES "x64/lib" "Lib" ""
            PATHS "${TWINCAT_DIR}/AdsApi/TcAdsDll")

    mark_as_advanced(ADS_LIBRARY)
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADS
        REQUIRED_VARS ADS_LIBRARY
        ADS_INCLUDE_DIR)

if (ADS_FOUND)
    set(ADS_INCLUDE_DIRS "${ADS_INCLUDE_DIR}")
    set(ADS_LIBRARIES "${ADS_LIBRARY}")

    if (NOT TARGET ADS::ADS)

        add_library(ADS::ADS UNKNOWN IMPORTED)

        set_target_properties(ADS::ADS PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${ADS_INCLUDE_DIRS}")

        set_target_properties(ADS::ADS PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                IMPORTED_LOCATION "${ADS_LIBRARY}")
    endif ()
endif ()
