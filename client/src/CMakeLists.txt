cmake_minimum_required(VERSION 3.5)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

set(GRPC_CLIENT_DIR grpc-client)
set(CLIENT_CONFIG_DIR client-config)
set(PROTO_DIR proto)

set(CLIENT_SOURCES
    client.cpp
    ${GRPC_CLIENT_DIR}/grpc_client.h
    ${GRPC_CLIENT_DIR}/grpc_client.cpp
    ${CLIENT_CONFIG_DIR}/client_config.h
    ${CLIENT_CONFIG_DIR}/client_config.cpp
)

add_executable(client ${CLIENT_SOURCES})

target_link_libraries(client PRIVATE proto-objects)
