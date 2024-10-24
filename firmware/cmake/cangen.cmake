list(APPEND CAN_DEPENDENCIES "${CMAKE_CURRENT_SOURCE_DIR}/config.yaml")

cmake_language(GET_MESSAGE_LOG_LEVEL LOG_LEVEL)

# cangen is provided by racecar/scripts/cangen
add_custom_target(
    generated_can
    COMMAND "cangen" ${CMAKE_CURRENT_SOURCE_DIR} "--log-level=${LOG_LEVEL}"
    DEPENDS ${CAN_DEPENDENCIES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT Generating CAN code from DBCs
)

add_dependencies(main generated_can)

# Link ETL which is needed for generated/can/msg_registry.h
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)