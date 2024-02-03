cmake_minimum_required(VERSION 3.5)

project(log_format VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

set(LOG_FROMAT_SOURCES
    log_format.h
    log_format.cpp
)

add_library(log_format ${LOG_FROMAT_SOURCES})

target_compile_definitions(log_format PUBLIC SPDLOG_FMT_EXTERNAL) 
target_include_directories(log_format PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(log_format PRIVATE proto-objects)
