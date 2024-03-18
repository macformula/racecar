# Blake Freer
# January 6, 2023

# The target executable 'main' is created in the master CMakeLists. Do not change its name.
# We only need to add the source code files and include directories.

target_sources(main
PRIVATE
	main.cc
)

target_include_directories(main
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_library(controls)

target_include_directories(controls
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/vehicle_control_system/src/vehicle_control_system-autogen/controller_autogen_ert_rtw
)

FILE(GLOB SRC_CONTROLS CONFIGURE_DEPENDS
	"vehicle_control_system/src/vehicle_control_system-autogen/controller_autogen_ert_rtw/*.c"
)

message(STATUS ${SRC_CONTROLS})

target_sources(controls
PUBLIC
	${SRC_CONTROLS}
)

target_link_libraries(main PUBLIC controls)


# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.