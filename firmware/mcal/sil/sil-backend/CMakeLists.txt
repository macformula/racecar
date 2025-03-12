add_library(sil)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

target_include_directories(sil
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${Protobuf_LIBRARIES}
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET sil LANGUAGE CPP PROTOS signals.proto PROTOC_OUT_DIR ".")
protobuf_generate(
    TARGET sil
    LANGUAGE grpc
    PROTOS signals.proto
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PROTOC_OUT_DIR "."
)

# Add source files
target_sources(sil PRIVATE
    sil_client.cc
    signals.pb.cc
    signals.grpc.pb.cc
)

target_link_libraries(sil
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
)
