cmake_minimum_required(VERSION 3.5)

project(dispatch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(manager/src)
add_subdirectory(client/src)
add_subdirectory(broker/src)
add_subdirectory(worker/src)
