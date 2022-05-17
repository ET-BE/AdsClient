project(AdsExamples)
cmake_minimum_required(VERSION 3.5)

if (NOT TARGET AdsClient)
    find_package(AdsClient)
endif()

add_subdirectory(basic)
add_subdirectory(symbols)
