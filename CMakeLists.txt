cmake_minimum_required(VERSION 3.18.4 FATAL_ERROR)

project(Match LANGUAGES C)

if(${CMAKE_BUILD_TYPE} MATCHES ^DEBUG$|^Debug$|^debug$)
	add_definitions(-DDEBUG)
endif()

include(FetchContent)

option(SUPPORT_JPG "Support loading JPEG images" OFF)

FetchContent_Declare(
    extern_sdl2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG main)

FetchContent_Declare(
    extern_sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.0.18)

FetchContent_MakeAvailable(extern_sdl2_image extern_sdl2)

add_executable(
	Match 
	src/match.c
)

target_include_directories(
	Match PRIVATE
	${extern_sdl2_SOURCE_DIR}/include
	${extern_sdl2_image_SOURCE_DIR}
)

set_target_properties(
	Match PROPERTIES
    C_STANDARD 90
    C_EXTENSIONS ON
)

target_link_libraries(
	Match 
	SDL2
	SDL2_image
)

