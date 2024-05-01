add_library(sil)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

target_include_directories(sil
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(sil ${Protobuf_LIBRARIES})
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET sil LANGUAGE CPP PROTOS signals.proto)
protobuf_generate(
    TARGET sil LANGUAGE grpc PROTOS signals.proto PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}" GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

# Add source files
target_sources(sil PRIVATE 
    sil_client.cc 
    signals.pb.cc 
    signals.grpc.pb.cc
)

# Set compiler and linker flags
target_compile_options(sil PRIVATE ${Protobuf_CFLAGS_OTHER})
target_link_libraries(sil PRIVATE ${Protobuf_LDFLAGS_OTHER})

# Print the necessary compiler and linker flags
message(STATUS "Protobuf includes: ${Protobuf_INCLUDE_DIRS}")
message(STATUS "Protobuf libs: ${Protobuf_LIBRARIES}")
message(STATUS "Protobuf compiler flags: ${Protobuf_CFLAGS_OTHER}")
message(STATUS "Protobuf linker flags: ${Protobuf_LDFLAGS_OTHER}")
