set(SRC
    otm8009a/otm8009a.c
    nt35510/nt35510.c
    ft6x06/ft6x06.c
)
set_source_files_properties(${SRC} PROPERTIES LANGUAGE C)
add_library(bspcomp INTERFACE)
target_sources(bspcomp INTERFACE ${SRC})
target_include_directories(bspcomp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
