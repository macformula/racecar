# Blake Freer
# December 18, 2023

# You probably shouldn't be modifying this file to adjust compilation as any changes
# will impact all projects and platforms.

# Use the adjacent Makefile to call CMake. It handles the gritty details to ensure
# options are properly set and the correct build directory is chosen.

cmake_minimum_required(VERSION 3.22)

# Get PROJECT option and ensure the folder exists

option(PROJECT "Project folder to compile" OFF)
if(NOT PROJECT)
	message(FATAL_ERROR "Must select a project with -D PROJECT=<project folder name>")
endif()

set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT}")
if(NOT (EXISTS "${DIR_PROJECT}" AND IS_DIRECTORY "${DIR_PROJECT}"))
	message(FATAL_ERROR "Invalid project \"${PROJECT}\". The directory ${DIR_PROJECT} does not exist.")
endif()

# Parse PLATFORM option and ensure it exists as a subdirectory of PROJECT/mcal

option(PLATFORM "Platform to compile for" OFF)
if(NOT PLATFORM)
	message(FATAL_ERROR "Must select a platform with -D PLATFORM=<platform name>")
endif()

set(DIR_PLATFORM "${DIR_PROJECT}/platforms/${PLATFORM}")
if(NOT (EXISTS "${DIR_PLATFORM}" AND IS_DIRECTORY "${DIR_PLATFORM}"))
	message(FATAL_ERROR "Invalid platform \"${PLATFORM}\". See ${DIR_PROJECT}/platforms for available platforms.")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Options are parsed. Start building
message("Building \"${PROJECT}\" for \"${PLATFORM}\"")

set(DIR_MCAL_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/mcal/${PLATFORM})
set(CMAKE_TOOLCHAIN_FILE ${DIR_MCAL_PLATFORM}/Toolchain.cmake)  # sets the compiler, linker, etc based on the Toolchain.cmake file

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)  # required for c++ concepts
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)

project("${PROJECT}-${PLATFORM}")

add_executable(main)

add_subdirectory(${DIR_PROJECT})  # provides "main" (executable)
add_subdirectory(${DIR_PLATFORM})  # provides "bindings" (library)

target_include_directories(main
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/etl/include/
	${DIR_PLATFORM}  # provides the platform-specific bindings header
)

target_include_directories(bindings
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}  # provides app.h
)

target_link_libraries(main
PRIVATE
	bindings
)

set(POSTBUILD_FILE ${DIR_MCAL_PLATFORM}/PostBuild.cmake)
if(EXISTS "${POSTBUILD_FILE}" AND NOT IS_DIRECTORY "${POSTBUILD_FILE}")
	include("${POSTBUILD_FILE}")
else()
	message(FATAL_ERROR "${POSTBUILD_FILE} does not exist. If no postbuild actions are required, create the file and leave it empty.")
endif()