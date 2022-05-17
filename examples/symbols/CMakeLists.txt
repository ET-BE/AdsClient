add_executable(example_symbols main.cpp)

target_link_libraries(example_symbols PUBLIC
        ADS::AdsClient)
