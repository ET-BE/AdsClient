add_library(AdsClient STATIC
        AdsClient.cpp
        ../include/AdsClient/AdsSymbol_impl.h AdsErrors.cpp)

target_include_directories(AdsClient PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>)

add_library(ADS::AdsClient ALIAS AdsClient)

target_link_libraries(AdsClient PUBLIC ADS::ADS)
