if(TESTING_ENABLED)
    target_sources(tests
        PRIVATE
        test_clamper.cc
        test_constant.cc
        test_linear_map.cc
        test_lookup_table.cc
        test_mapper.cc
        test_quadratic_map.cc
    )
endif()