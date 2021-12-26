cmake_minimum_required(VERSION 3.18.4 FATAL_ERROR)

project(Match)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Match src/main.c)

target_link_libraries(Match ${SDL2_LIBRARIES})