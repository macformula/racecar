# Samuel Parent
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
	COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/../scripts/cangen/main.py" "--project=\"IoCheckoutFc\""
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
)

# Link ETL which is needed for generated/can/msg_registry.h
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)

# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.