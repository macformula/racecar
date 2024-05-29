# Blake Freer
# January 8, 2024

target_sources(bindings
PUBLIC
	bindings.cc
)

target_include_directories(bindings PUBLIC ${DIR_PROJECT}/inc) # access to app.h

add_subdirectory(cubemx)

# must be public so that the 'main' executable can see its headers when including bindings.h
target_link_libraries(bindings PUBLIC driver mcal-stm32f767)

add_library(os)
target_sources(os PUBLIC os.cc)
target_link_libraries(os PUBLIC mcal-stm32f767 driver)