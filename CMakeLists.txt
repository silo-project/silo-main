cmake_minimum_required(VERSION 3.15)
project(silo-main C)

set(CMAKE_VERBOSE_MAKEFILE true)

add_subdirectory(src)

add_compile_options(-lpthread -g -O3)

target_link_libraries(silo-main pthread)
