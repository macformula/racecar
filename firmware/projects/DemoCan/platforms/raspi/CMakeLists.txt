target_sources(bindings
	PRIVATE
	bindings.cc
)

target_include_libraries(bindings PUBLIC ${CMAKE_SOURCE_DIR})
