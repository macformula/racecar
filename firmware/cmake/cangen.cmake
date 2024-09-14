find_package(Python3 COMPONENTS Interpreter)
message(${Python3_EXECUTABLE})

if(NOT ${Python3_FOUND})
	message(FATAL_ERROR "Python 3 executable not found")
endif()

FILE(GLOB_RECURSE CAN_DEPENDENCIES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/../scripts/cangen/*" "${CMAKE_SOURCE_DIR}/dbcs/*")

list(APPEND CAN_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/config.yaml")

cmake_language(GET_MESSAGE_LOG_LEVEL LOG_LEVEL)

add_custom_target(
    generated_can
	COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/../scripts/cangen/main.py" "--project=\"${CMAKE_CURRENT_SOURCE_DIR}\"" "--log-level=${LOG_LEVEL}"
	DEPENDS ${CAN_DEPENDENCIES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT Generating CAN code from DBCs
)

# Link ETL which is needed for generated/can/msg_registry.h
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)