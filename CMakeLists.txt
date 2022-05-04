project(AdsClient)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

find_package(ADS REQUIRED)

add_subdirectory(src)

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
        NAMESPACE AdsClient::
        DESTINATION ${ConfigPackageLocation})

install(DIRECTORY include/AdsClient
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(AdsClient-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/AdsClient-config.cmake
        INSTALL_DESTINATION ${ConfigPackageLocation}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AdsClient-config.cmake"
              "${CMAKE_SOURCE_DIR}/cmake/FindADS.cmake"
        DESTINATION ${ConfigPackageLocation})
