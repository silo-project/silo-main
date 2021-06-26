cmake_minimum_required(VERSION 3.15)
project(silo C)

set(CMAKE_VERBOSE_MAKEFILE true)
set(PATH_LIB src/lib)

add_subdirectory(src)

add_compile_options(-lpthread -g -O0 -std=c11 -fsyntax-only)



target_link_libraries(
    silo
    pthread
)
