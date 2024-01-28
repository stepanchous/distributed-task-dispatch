cmake_minimum_required(VERSION 3.5)

project(manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

find_package(Boost 1.78.0 COMPONENTS REQUIRED graph)
find_package(nlohmann_json REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(gRPC REQUIRED)

# Build proto
set(PROTO_DIR proto)

add_library(proto-objects-server OBJECT ${PROTO_DIR}/ast_json.proto)

target_link_libraries(proto-objects-server PRIVATE protobuf::libprotobuf gRPC::grpc++)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto")

protobuf_generate(
    TARGET proto-objects-server
    OUT_VAR PROTO_GENERATED_FILES
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)

protobuf_generate(
    TARGET proto-objects-server
    OUT_VAR PROTO_GENERATED_FILES
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto"
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)

target_include_directories(proto-objects-server PRIVATE "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

# Build manager
include_directories(lib)

set(DECOMPOSITION_DIR decomposition)
set(DOMAIN_DIR domain)
set(GRPC_SERVER_DIR grpc-server)

set(MANAGER_SOURCES
  manager.cpp
  ${DECOMPOSITION_DIR}/ast.h
  ${DECOMPOSITION_DIR}/ast.cpp
  ${DECOMPOSITION_DIR}/expression.h
  ${DECOMPOSITION_DIR}/expression.cpp
  ${DECOMPOSITION_DIR}/dag_visitor.h
  ${DECOMPOSITION_DIR}/dag_visitor.cpp
  ${DECOMPOSITION_DIR}/expression_visitors.h
  ${DECOMPOSITION_DIR}/expression_visitors.cpp
  ${DOMAIN_DIR}/domain.h
  ${DOMAIN_DIR}/domain.cpp
  ${GRPC_SERVER_DIR}/grpc_server.h
  ${GRPC_SERVER_DIR}/grpc_server.cpp
)

add_executable(manager ${PROTO_SRC} ${PROTO_HDRS} ${MANAGER_SOURCES})

target_link_libraries(manager PRIVATE proto-objects-server)
target_link_libraries(manager PRIVATE Boost::graph)
target_link_libraries(manager PRIVATE nlohmann_json::nlohmann_json)
