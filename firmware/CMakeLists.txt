# Blake Freer
# December 18, 2023

# You probably shouldn't be modifying this file to adjust compilation as any
# changes will impact all projects and platforms.

# Use the adjacent Makefile to call CMake. It handles the gritty details to
# ensure options are properly set and the correct build directory is chosen.

cmake_minimum_required(VERSION 3.27)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Validate required arguments
foreach(arg IN ITEMS PROJECT PLATFORM PROJECT_NAME)
    if(NOT DEFINED ${arg})
        if(DEFINED ENV{${arg}})
            set(${arg} $ENV{${arg}})
        else()
            message(FATAL_ERROR "Must provide ${arg} (as a CMake variable or environment variable)")
        endif()
    endif()
endforeach()

# Check that the project and platform folders exist.
set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT}")
set(DIR_PLATFORM "${DIR_PROJECT}/platforms/${PLATFORM}")
set(MCAL_CONF_FILE "${DIR_PLATFORM}/mcal_conf.cmake")

foreach(dir IN ITEMS DIR_PROJECT DIR_PLATFORM MCAL_CONF_FILE)
    if(NOT EXISTS "${${dir}}")
        message(FATAL_ERROR "${${dir}} does not exist.")
    endif()
endforeach()

include(${MCAL_CONF_FILE})
if(NOT DEFINED MCAL)
    message(FATAL_ERROR "MCAL not defined in ${MCAL_CONF_FILE}")
endif()

set(DIR_MCAL "${CMAKE_CURRENT_SOURCE_DIR}/mcal/${MCAL}")
if(NOT EXISTS "${DIR_MCAL}")
    message(FATAL_ERROR "MCAL directory does not exist: ${DIR_MCAL}")
endif()

set(POSTBUILD_FILE "${DIR_MCAL}/PostBuild.cmake")

message(STATUS "Building ${PROJECT} for ${PLATFORM} with MCAL ${MCAL}")

# clangd export settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

# Project setup
project(${PROJECT_NAME})
enable_language(C CXX ASM)

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)

# Enable testing for CLI/Linux platforms
if(MCAL STREQUAL "cli" OR MCAL STREQUAL "linux")
    set(TESTING_ENABLED ON)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/googletest/CMakeLists.txt")
        message(FATAL_ERROR "GoogleTest not found. Run: git submodule update --init --recursive")
    endif()
    
    add_subdirectory(third-party/googletest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    add_executable(tests)

    target_link_libraries(tests PUBLIC GTest::gtest_main)
    target_link_libraries(tests PRIVATE shared etl)
    
    include(GoogleTest)
    gtest_discover_tests(tests)
    
    # Run tests after building
    add_custom_command(
        TARGET tests POST_BUILD
        COMMAND tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running unit tests"
    )
endif()

# Create executables
add_executable(main)

# Add subdirectories and link libraries
add_subdirectory(shared)
add_subdirectory(${DIR_MCAL})
add_subdirectory(${DIR_PROJECT})
add_subdirectory(${DIR_PLATFORM})
add_subdirectory(third-party/etl)

# Link libraries to main executable
target_link_libraries(main PRIVATE shared etl)
target_compile_options(main PRIVATE -Wno-psabi)

# Include platform-specific post-build commands
include(${POSTBUILD_FILE})
