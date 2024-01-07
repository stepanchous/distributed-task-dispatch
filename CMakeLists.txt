cmake_minimum_required(VERSION 3.5)

project(dispatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(lib)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Werror)
add_compile_options(-pedantic)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address)
endif()

set(DECOMPOSITION_DIR decomposition)
set(DOMAIN_DIR domain)

set(PROJECT_SOURCES
  main.cpp
  ${DECOMPOSITION_DIR}/ast.h
  ${DECOMPOSITION_DIR}/ast.cpp
  ${DECOMPOSITION_DIR}/expression.h
  ${DECOMPOSITION_DIR}/expression.cpp
  ${DOMAIN_DIR}/domain.h
  ${DOMAIN_DIR}/domain.cpp)

add_executable(dispatch
    ${PROJECT_SOURCES})


