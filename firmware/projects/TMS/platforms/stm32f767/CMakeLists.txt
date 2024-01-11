# Blake Freer
# January 8, 2024

add_library(bindings)

target_sources(bindings
PUBLIC
	bindings.cc
)

add_subdirectory(cubemx)

# must be public so that the 'main' executable can see its headers when including bindings.h
target_link_libraries(bindings PUBLIC driver)
