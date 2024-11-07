# Blake Freer
# January 6, 2023

# The target executable 'main' is created in the master CMakeLists. Do not change its name.
# We only need to add the source code files and include directories.

include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main
	PRIVATE
	main.cc
)

target_include_directories(main
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/generated/can
)

# Compile and link the control system files.

add_library(controls)

set(CTRL_SYS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_control_system/src/vehicle_control_system-autogen/controller_autogen_ert_rtw")
target_include_directories(controls PUBLIC ${CTRL_SYS_DIR})

FILE(GLOB SRC_CONTROLS CONFIGURE_DEPENDS "${CTRL_SYS_DIR}/*.cpp")
message(STATUS "Controls sources: ${SRC_CONTROLS}")

target_sources(controls
PUBLIC
	${SRC_CONTROLS}
)

target_link_libraries(main PUBLIC controls)


# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.