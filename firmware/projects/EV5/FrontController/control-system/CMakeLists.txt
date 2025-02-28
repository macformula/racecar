add_library(control-system INTERFACE)
add_subdirectory(src)
target_include_directories(control-system INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(TESTING_ENABLED)
    target_link_libraries(tests PRIVATE control-system)

    file(GLOB CONTROL_SYS_TEST_SRC "test/test*.cc")
    target_sources(tests PRIVATE ${CONTROL_SYS_TEST_SRC})
endif()