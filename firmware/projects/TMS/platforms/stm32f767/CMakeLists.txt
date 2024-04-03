# Blake Freer
# January 8, 2024
add_library(os)

target_sources(bindings
	PUBLIC
	bindings.cc
)

target_sources(os
	PUBLIC
	os.cc
)

add_subdirectory(cubemx)

# must be public so that the 'main' executable can see its headers when including bindings.h
target_link_libraries(bindings PUBLIC driver)
target_link_libraries(os PUBLIC driver)
