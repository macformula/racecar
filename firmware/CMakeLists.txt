# Blake Freer
# December 18, 2023

# You probably shouldn't be modifying this file to adjust compilation as any
# changes will impact all projects and platforms.

# Use the adjacent Makefile to call CMake. It handles the gritty details to
# ensure options are properly set and the correct build directory is chosen.

cmake_minimum_required(VERSION 3.27)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

message("Build type: ${CMAKE_BUILD_TYPE}")

# Get PROJECT and PLATFORM options
# Note that the neither can be called "OFF" as CMake will think the variable is
# unset.
option(PROJECT "Project folder to compile" OFF)
option(PLATFORM "Platform to compile for" OFF)
option(PROJECT_NAME "Project name." OFF)

message(CHECK_START "Validating input arguments")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

# Ensure options were provided
if(NOT PROJECT)
    message(FATAL_ERROR "Must provide PROJECT.")
endif()

if(NOT PLATFORM)
    message(FATAL_ERROR "Must provide PLATFORM.")
endif()

if(NOT PROJECT_NAME)
    message(FATAL_ERROR "Must provide PROJECT_NAME.")
endif()

# Check that the project and platform folders exist.
set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT}")
assert_exists(DIR PATH ${DIR_PROJECT}
    START "Verifying ${PROJECT} is a project"
    PASS "Valid"
    FAIL "Invalid"
)

set(DIR_PLATFORM "${DIR_PROJECT}/platforms/${PLATFORM}")
assert_exists(DIR PATH ${DIR_PLATFORM}
    START "Verifying ${PLATFORM} is a platform under ${PROJECT}"
    PASS "Valid"
    FAIL "Invalid"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Success")

message(CHECK_START "Validating the MCAL Platform")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(MCAL_CONF_FILE "${DIR_PLATFORM}/mcal_conf.cmake")
assert_exists(FILE PATH ${MCAL_CONF_FILE}
    START "Searching for mcal configuration file"
    PASS "Found"
    FAIL "Not found"
)

include(${MCAL_CONF_FILE}) # Should define MCAL

if(NOT DEFINED MCAL)
    message(FATAL_ERROR "Expected MCAL to be defined in \"${MCAL_CONF_FILE}\".")
endif()

set(DIR_MCAL "${CMAKE_CURRENT_SOURCE_DIR}/mcal/${MCAL}")
assert_exists(DIR PATH ${DIR_MCAL}
    START "Verifying the ${MCAL} MCAL exists"
    PASS "Valid MCAL"
    FAIL "Invalid MCAL specified in ${MCAL_CONF_FILE}."
)

set(POSTBUILD_FILE "${DIR_MCAL}/PostBuild.cmake")
assert_exists(FILE PATH ${POSTBUILD_FILE}
    START "Finding MCAL postbuild file"
    PASS "Found"
    FAIL "Not found. If no postbuild actions are required, create the file\n"
    ${POSTBUILD_FILE} "\nand leave it empty."
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Success")

# Options are parsed. Start building
message(STATUS "Building \"${PROJECT}\" for \"${PLATFORM}\" with mcal \"${MCAL}\".")

# clangd export settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

project(${PROJECT_NAME})

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)

# ------------ Testing ------------
set(TESTING_ENABLED OFF)

if(MCAL STREQUAL "cli" OR MCAL STREQUAL "linux")
    set(TESTING_ENABLED ON) # checked by subfolders to add their tests

    set(GTEST_DIR "third-party/googletest")

    if(NOT EXISTS ${GTEST_DIR}/CMakeLists.txt)
        message(FATAL_ERROR
            "GoogleTest was not found at ${GTEST_DIR}.\n"
            "Run `git submodule update --init --recursive`")
    endif()

    add_subdirectory(${GTEST_DIR})
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    add_executable(tests)
else()
    message(STATUS "Skipping tests - GoogleTest is not supported on this platform.")
endif()

add_executable(main)

add_subdirectory(shared) # provides "shared" (library)
add_subdirectory(${DIR_MCAL}) # provides "mcal-<name>" (library)

add_subdirectory(${DIR_PROJECT}) # modifies "main" (executable)
add_subdirectory(${DIR_PLATFORM}) # modifies "bindings" (library)

target_link_libraries(main PRIVATE shared)

target_compile_options(main PRIVATE -Wno-psabi)

include("${POSTBUILD_FILE}")

if(TESTING_ENABLED)
    target_link_libraries(tests PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(tests)

    set(UNIT_TEST tests)
    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${UNIT_TEST}
    )

    target_link_libraries(tests PRIVATE shared)
endif()