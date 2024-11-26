# Blake Freer
# January 6, 2023

# The target executable 'main' is created in the master CMakeLists. Do not change its name.
# We only need to add the source code files and include directories.

# add_subdirectory(platforms/stm32f767)  # provides "bindings" (library)

target_sources(main PRIVATE main.cc)

# Notice that we don't include any mcal/ subdirectory in this CMake file.
# The master CMakeLists handles platform selection and library linking.