cmake_minimum_required(VERSION 2.8)

project(wheel C CXX)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(net)
