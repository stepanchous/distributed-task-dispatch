cmake_minimum_required(VERSION 3.5)

project(dispatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

find_package(spdlog REQUIRED)

set(DB_CLIENT_SOURCES
    database_client.h
    database_client.cpp
)

add_library(db_client ${DB_CLIENT_SOURCES})

target_link_libraries(db_client PRIVATE proto-objects)
target_link_libraries(db_client PRIVATE spdlog::spdlog)
