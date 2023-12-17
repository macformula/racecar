# Blake Freer
# December 17, 2023

add_library(tms_main)

target_sources(tms_main
  PUBLIC
    main.cc
)

target_include_directories(tms_main
  PUBLIC
    mcal/windows
	../
)

