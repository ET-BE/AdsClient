cmake_minimum_required(VERSION "3.10")
project("AdsClient")

file(GLOB project_SRC
	"src/*.h"
    "src/tcAdsClient.cpp"
)

add_library("${PROJECT_NAME}" SHARED ${project_SRC})

target_include_directories("${PROJECT_NAME}" PUBLIC
	${PROJECT_SOURCE_DIR}/src
	$ENV{TWINCAT3DIR}/../AdsApi/TcAdsDll/Include
)

target_link_directories( "${PROJECT_NAME}" PRIVATE
		$ENV{TWINCAT3DIR}/../AdsApi/TcAdsDll/x64
		$ENV{TWINCAT3DIR}/../AdsApi/TcAdsDll/x64/lib)

target_link_libraries("${PROJECT_NAME}" TcAdsDll)

