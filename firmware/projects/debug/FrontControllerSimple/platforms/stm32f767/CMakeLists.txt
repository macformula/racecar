# Blake Freer
# January 8, 2024

add_subdirectory(cubemx)

target_sources(bindings PUBLIC bindings.cc)
target_link_libraries(bindings PUBLIC driver mcal-stm32f767)

target_include_directories(bindings PUBLIC ${DIR_PROJECT}/inc) # access to bindings.hpp

add_library(os)
target_sources(os PUBLIC os.cc)
target_link_libraries(os PUBLIC driver mcal-stm32f767)