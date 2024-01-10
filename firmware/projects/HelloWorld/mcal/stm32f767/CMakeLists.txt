# Blake Freer
# January 8, 2024


add_library(bindings)

target_sources(bindings
PUBLIC
	bindings.cc
)
target_include_directories(bindings
PUBLIC
	cubemx/Inc
)

add_subdirectory(cubemx)
# must be public so that the 'main' executable can use its headers
target_link_libraries(bindings PUBLIC driver)
