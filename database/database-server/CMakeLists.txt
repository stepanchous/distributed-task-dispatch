cmake_minimum_required(VERSION 3.5)

project(dispatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

find_package(SQLiteCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(gRPC REQUIRED)

set(DB_SERVER_IMPL_DIR database-server-impl)

set(DB_SERVER_SOURCES
    database_server.cpp
    ${DB_SERVER_IMPL_DIR}/database_server.h
    ${DB_SERVER_IMPL_DIR}/database_server.cpp
)

add_executable(db_server ${DB_SERVER_SOURCES})

target_link_libraries(db_server PRIVATE proto-objects)
target_link_libraries(db_server PRIVATE SQLiteCpp)
target_link_libraries(db_server PRIVATE spdlog::spdlog)
target_link_libraries(db_server PRIVATE grpc++_reflection)
