add_library(sil)

target_sources(sil PRIVATE sil_client.cc)
include_directories("/usr/local/lib/flatbuffers")
include_directories("/usr/local/lib/include/flatbuffers")
find_package(flatbuffers REQUIRED)
find_package(Flatbuffers REQUIRED)


#target_link_libraries(sil PRIVATE flatbuffers)
#target_link_libraries(sil
#    PUBLIC   
#)
