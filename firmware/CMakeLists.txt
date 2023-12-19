# Blake Freer
# December 18, 2023

cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)

# Get PROJECT option and ensure the folder exists

option(PROJECT "Project folder to compile" OFF)
if(NOT PROJECT)
	message(FATAL_ERROR "Must select a project with -D PROJECT=<project folder name>")
endif()

set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")
if(NOT (EXISTS "${DIR_PROJECT}" AND IS_DIRECTORY "${DIR_PROJECT}"))
	message(FATAL_ERROR "Invalid project \"${PROJECT}\". The directory ${DIR_PROJECT} does not exist.")
endif()

# Parse PLATFORM option and ensure it exists as a subdirectory of PROJECT/mcal

option(PLATFORM "Platform to compile for" OFF)
if(NOT PLATFORM)
	message(FATAL_ERROR "Must select a platform with -D PLATFORM=<platform name>")
endif()

set(DIR_PLATFORM "${DIR_PROJECT}/mcal/${PLATFORM}")
if(NOT (EXISTS "${DIR_PLATFORM}" AND IS_DIRECTORY "${DIR_PLATFORM}"))
	message(FATAL_ERROR "Invalid platform \"${PLATFORM}\". See ${DIR_PROJECT}/mcal for available platforms.")
endif()

# Options are parsed -> Start building

message(STATUS "Building \"${PROJECT}\" for \"${PLATFORM}\"")
project("${PROJECT}-${PLATFORM}")

add_subdirectory(${DIR_PROJECT})  # provides "main" (executable)
add_subdirectory(${DIR_PLATFORM})  # provides "bindings" (library)

target_include_directories(main
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
	${DIR_PLATFORM}
)

target_include_directories(bindings
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(main
PRIVATE
	bindings
)