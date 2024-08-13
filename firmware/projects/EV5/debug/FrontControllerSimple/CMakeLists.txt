# Blake Freer
# January 6, 2023

# The target executable 'main' is created in the master CMakeLists. Do not change its name.
# We only need to add the source code files and include directories.

find_package(Python3 COMPONENTS Interpreter)
message(${Python3_EXECUTABLE})

if(NOT ${Python3_FOUND})
	message(FATAL_ERROR "Python 3 executable not found")
endif()

FILE(GLOB_RECURSE CAN_DEPENDENCIES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/../scripts/cangen/*" "${CMAKE_SOURCE_DIR}/dbcs/*")

list(APPEND CAN_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/config.yaml")

add_custom_target(
    generated_can
	COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/../scripts/cangen/main.py" "--project=\"EV5/debug/FrontControllerSimple\""
	DEPENDS ${CAN_DEPENDENCIES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT Generating CAN code from DBCs
)

target_sources(main
	PRIVATE
	main.cc
)
add_dependencies(main generated_can)

target_include_directories(main
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/generated/can
)

# Compile and link the control system files.

add_library(controls)

set(CTRL_SYS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_control_system/controller_autogen_ert_rtw")
target_include_directories(controls PUBLIC ${CTRL_SYS_DIR})

FILE(GLOB SRC_CONTROLS CONFIGURE_DEPENDS "${CTRL_SYS_DIR}/*.cpp")
message(STATUS "Controls sources: ${SRC_CONTROLS}")

target_sources(controls
PUBLIC
	${SRC_CONTROLS}
)

target_link_libraries(main PUBLIC controls)

# Link ETL which is needed for generated/can/msg_registry.h
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)

# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.