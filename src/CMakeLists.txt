
add_library(AdsClient STATIC
        AdsClient.cpp)

target_include_directories(AdsClient PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(AdsClient PUBLIC ADS::ADS)
