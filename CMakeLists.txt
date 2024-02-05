cmake_minimum_required(VERSION 3.5)

project(dispatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_BINARY_DIR ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_definitions(SPDLOG_FMT_EXTERNAL)

include_directories(lib)
include_directories(common)
include_directories(database)

add_subdirectory(proto)
add_subdirectory(common)
add_subdirectory(database/database-server)
add_subdirectory(database/database-client)
add_subdirectory(broker/src)
add_subdirectory(worker/src)
add_subdirectory(manager/src)
add_subdirectory(client/src)
