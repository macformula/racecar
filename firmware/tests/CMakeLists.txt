cmake_minimum_required(VERSION 3.14)
project(fw_test)

# GoogleTest requires at least C++14
# racecar/firmware requires at least C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(googletest)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

# Find all .cc files under tests/firmware
FILE(GLOB_RECURSE ALL_CC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/firmware/**.cc)

add_executable(
	fw_test
	${ALL_CC_SOURCES}
)
target_link_libraries(
	fw_test
	gtest_main
)

include(GoogleTest)

gtest_discover_tests(fw_test)
