target_include_directories(bindings
    PRIVATE
    "$<TARGET_PROPERTY:mcal-raspi,INTERFACE_INCLUDE_DIRECTORIES>"
)

target_link_libraries(bindings PRIVATE mcal-raspi)

target_sources(bindings PRIVATE bindings.cc)
