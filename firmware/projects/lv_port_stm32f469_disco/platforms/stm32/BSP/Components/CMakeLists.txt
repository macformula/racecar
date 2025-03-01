set(SRC
    nt35510/nt35510.c
)

set_source_files_properties(${SRC} PROPERTIES LANGUAGE CXX)
add_library(bspcomp INTERFACE)
target_sources(bspcomp INTERFACE ${SRC})
target_include_directories(bspcomp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
