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

# Ensure options were provided
if(NOT PROJECT)
    message(FATAL_ERROR
        "Must select a project with -D PROJECT=<project name>")
endif()

if(NOT PLATFORM)
    message(FATAL_ERROR
        "Must select a platform with -D PLATFORM=<platform name>")
endif()

# Check that the project and platform folders exist.
set(DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT}")
set(DIR_PLATFORM "${DIR_PROJECT}/platforms/${PLATFORM}")

directory_exists(out ${DIR_PROJECT})

if(NOT ${out})
    message(FATAL_ERROR "Invalid project \"${PROJECT}\".")
endif()

directory_exists(out ${DIR_PLATFORM})

if(NOT ${out})
    message(FATAL_ERROR "Invalid platform \"${PLATFORM}\".")
endif()

# There must be an mcal_conf,cmake file in the platform folder to specify which
# mcal folder will be included.
set(MCAL_CONF_FILE ${DIR_PLATFORM}/mcal_conf.cmake)

file_exists(out ${MCAL_CONF_FILE})

if(NOT ${out})
    message(FATAL_ERROR "Expected to find \"${MCAL_CONF_FILE}\".")
endif()

include(${MCAL_CONF_FILE}) # Should define MCAL

if(NOT DEFINED MCAL)
    message(FATAL_ERROR
        "Expected MCAL to be defined in \"${MCAL_CONF_FILE}\".")
endif()

set(DIR_MCAL ${CMAKE_CURRENT_SOURCE_DIR}/mcal/${MCAL})

directory_exists(out ${DIR_MCAL})

if(NOT ${out})
    string(CONCAT errmsg
        "Invalid mcal \"${MCAL}\". "
        "See ${CMAKE_CURRENT_SOURCE_DIR}/mcal for available mcals."
    )
    message(FATAL_ERROR ${errmsg})
endif()

# Options are parsed. Start building
message("Building \"${PROJECT}\" for \"${PLATFORM}\" with mcal \"${MCAL}\".")

# Sets the compiler, linker, etc based on the mcal's Toolchain.cmake file
set(CMAKE_TOOLCHAIN_FILE ${DIR_MCAL}/Toolchain.cmake)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20) # required for c++ concepts
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)
set(CMAKE_CXX_EXTENSIONS ON)

project("${PROJECT}-${PLATFORM}")

# clangd export settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

add_executable(main)
add_library(bindings)

add_subdirectory(shared) # provides "shared" (library)
add_subdirectory(${DIR_MCAL}) # provides "mcal-<name>" (library)

add_subdirectory(${DIR_PROJECT}) # modifies "main" (executable)
add_subdirectory(${DIR_PLATFORM}) # modifies "bindings" (library)

set(DIR_ETL "${CMAKE_CURRENT_SOURCE_DIR}/third-party/etl/include")
target_include_directories(main
    PUBLIC
    ${DIR_ETL}
)

target_link_libraries(main PRIVATE shared)
target_link_libraries(main PRIVATE bindings)

# Only link/include RTOS if the project supports RTOS
# The project needs to provide an underlying OS target
# in the platform/${PLATFORM_NAME}/CMakeLists.txt
if(TARGET os)
    target_link_libraries(main PRIVATE os)
endif()

set(POSTBUILD_FILE ${DIR_MCAL}/PostBuild.cmake)

file_exists(out ${POSTBUILD_FILE})

if(${out})
    include("${POSTBUILD_FILE}")
else()
    string(CONCAT errmsg
        "${POSTBUILD_FILE} does not exist. "
        "If no postbuild actions are required, create the file and leave it "
        "empty."
    )
    message(FATAL_ERROR ${errmsg})
endif()
