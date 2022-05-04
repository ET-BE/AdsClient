project(ExampleBasic)
cmake_minimum_required(VERSION 3.5)

find_package(AdsClient)

add_executable(example_basic main.cpp)

target_link_libraries(example_basic PUBLIC
        AdsClient::AdsClient)
