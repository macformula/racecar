add_subdirectory(mappers)

if(TESTING_ENABLED)
    target_sources(tests PRIVATE
        test_arrays.cc
        test_moving_average.cc
    )
endif()