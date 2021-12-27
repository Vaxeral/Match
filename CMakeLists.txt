cmake_minimum_required(VERSION 3.18.4 FATAL_ERROR)

project(Match C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

if(${CMAKE_BUILD_TYPE} MATCHES ^DEBUG$|^Debug$|^debug$)
	add_definitions(-DDEBUG)
endif()

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(
	Match 
	src/match.c
)

set_property(
	TARGET Match 
	PROPERTY C_STANDARD 89
	PROPERTY C_STANDARD_REQUIRED
)

target_link_libraries(Match ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})