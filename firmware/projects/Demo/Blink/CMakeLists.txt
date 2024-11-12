target_sources(main PUBLIC main.cc)
target_compile_definitions(main PRIVATE MAIN_PROGRAM)

target_sources(test_program PUBLIC test.cc)

# Add a test target for the test executable
add_test(NAME TestAddition COMMAND test_program)
