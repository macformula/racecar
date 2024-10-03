# Samuel Parent
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
)

# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.