target_include_directories(bindings
    PRIVATE
    "$<TARGET_PROPERTY:mcal-raspi,INTERFACE_INCLUDE_DIRECTORIES>"
)

target_sources(bindings
PRIVATE
	bindings.cc
)

target_include_directories(bindings PRIVATE ${DIR_PROJECT}/inc)

target_link_libraries(bindings PUBLIC mcal-raspi)