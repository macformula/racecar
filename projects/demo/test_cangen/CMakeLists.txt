include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main PRIVATE main.cc)

if(TESTING_ENABLED)
    file(GLOB CONTROL_SYS_TEST_SRC "test/test*.cc")
    target_sources(tests PRIVATE ${CONTROL_SYS_TEST_SRC})
    add_dependencies(tests generated_can)
endif()