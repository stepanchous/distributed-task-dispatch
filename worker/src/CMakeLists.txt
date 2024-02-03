cmake_minimum_required(VERSION 3.5)

project(worker VERSION 0.1 LANGUAGES CXX)

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

set(WORKER_SOURCES
    worker.cpp
)

add_executable(worker ${WORKER_SOURCES})

target_link_libraries(worker PRIVATE ${ZeroMQ_LIBRARY})
