# Blake Freer
# January 8, 2024

target_sources(bindings
PUBLIC
	bindings.cc
)

add_subdirectory(cubemx)

# must be public so that the 'main' executable can see its headers when including bindings.h
target_link_libraries(bindings PUBLIC driver)
