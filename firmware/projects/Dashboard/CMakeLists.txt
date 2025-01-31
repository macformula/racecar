set(COMMON_WARNINGS
    -Wall -Wextra
    -Wshadow -Wundef -Wmaybe-uninitialized -Wno-discarded-qualifiers
    -Wno-unused-function -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized
    -Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default
    -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wdouble-promotion -Wclobbered -Wdeprecated
    -Wempty-body -Wshift-negative-value -Wstack-usage=2048
    -Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith
)

add_compile_options(
    ${COMMON_WARNINGS}
)

add_definitions(-DSIMULATOR=1 -DLV_BUILD_TEST=0)

add_subdirectory(third-party/lvgl)
target_link_libraries(main PRIVATE lvgl)

add_subdirectory(third-party/lv_drivers)
target_link_libraries(main PRIVATE lv_drivers)
target_link_libraries(lv_drivers PRIVATE lvgl)
target_include_directories(lv_drivers PRIVATE third-party)

target_include_directories(main PRIVATE third-party)

target_sources(main PRIVATE main.cc)

add_subdirectory(src)

find_package(SDL2 REQUIRED)

target_link_libraries(main PRIVATE
    SDL2::SDL2
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})