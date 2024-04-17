target_sources(bindings
	PRIVATE
	bindings.cc
)

target_include_directories(bindings PUBLIC ${CMAKE_SOURCE_DIR})

add_library(os)
target_sources(os PUBLIC ${CMAKE_SOURCE_DIR}/mcal/raspi/os/tick_blocking.cc)
target_link_libraries(os PUBLIC mcal-raspi)