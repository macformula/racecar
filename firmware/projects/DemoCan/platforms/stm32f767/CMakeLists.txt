/// @author Clara Wong
/// @date 2024-03-20

add_library(bindings)

target_sources(bindings
PUBLIC
	bindings.cc
)

add_subdirectory(cubemx)

target_link_libraries(bindings PUBLIC driver)
