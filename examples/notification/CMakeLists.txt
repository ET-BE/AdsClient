add_executable(example_notification main.cpp)

target_link_libraries(example_notification PUBLIC
        ADS::AdsClient)
