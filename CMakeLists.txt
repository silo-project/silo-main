cmake_minimum_required(VERSION 3.15)
project(silo C)

set(CMAKE_VERBOSE_MAKEFILE true)

add_subdirectory(src)

add_compile_options(-lpthread -g)

target_link_libraries(silo -pthread)
