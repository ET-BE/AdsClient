project(AdsClient)

cmake_minimum_required(VERSION 3.5)

option(BUILD_EXAMPLES "Enable to also build the samples" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

find_package(ADS REQUIRED)

add_subdirectory(src)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/AdsClient/cmake)

#
# Install the library and the include directory.
# Also install the target exports and the custom *-config.cmake script, which is used to run FindADS for applications
# using this library.
#

install(TARGETS AdsClient
        EXPORT AdsClient-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT AdsClient-targets
        NAMESPACE ADS::
        DESTINATION ${ConfigPackageLocation})

install(DIRECTORY include/AdsClient
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/AdsClient-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/AdsClient-config.cmake
        INSTALL_DESTINATION ${ConfigPackageLocation}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AdsClient-config.cmake"
              "${CMAKE_SOURCE_DIR}/cmake/FindADS.cmake"
        DESTINATION ${ConfigPackageLocation})
