cmake_minimum_required(VERSION 3.5)

project(broker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-pedantic)

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_library(
    ZeroMQ_LIBRARY
    NAMES zmq
    PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
)

set(BROKER_IMPL_DIR broker-impl)

set(BROKER_SOURCES
    broker.cpp
    ${BROKER_IMPL_DIR}/broker_impl.h
    ${BROKER_IMPL_DIR}/broker_impl.cpp
)

add_executable(broker ${BROKER_SOURCES})

target_link_libraries(broker PRIVATE ${ZeroMQ_LIBRARY})
