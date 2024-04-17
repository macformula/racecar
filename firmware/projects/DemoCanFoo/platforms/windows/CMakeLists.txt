target_sources(bindings
PRIVATE
	bindings.cc
)

target_link_libraries(bindings PUBLIC mcal-windows)

add_library(os)
target_sources(os PUBLIC ${CMAKE_SOURCE_DIR}/mcal/windows/os/tick_blocking.cc)
target_link_libraries(os PUBLIC mcal-windows)