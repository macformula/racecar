if(TESTING_ENABLED)
    target_sources(tests PRIVATE
        test_lookup_table.cc
        test_moving_average.cc
    )
endif()