target_sources(bindings
	PRIVATE
	bindings.cc
)

target_include_directories(bindings PUBLIC ${CMAKE_SOURCE_DIR})