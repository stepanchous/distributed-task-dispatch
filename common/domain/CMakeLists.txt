cmake_minimum_required(VERSION 3.5)

project(domain VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

set(DOMAIN_SOURCES
    domain.h
    domain.cpp
)

add_library(domain ${DOMAIN_SOURCES})

target_include_directories(domain PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
