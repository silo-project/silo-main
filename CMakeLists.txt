cmake_minimum_required(VERSION 3.15)
project(silo C)

set(CMAKE_VERBOSE_MAKEFILE true)
set(PATH_LIB src/lib)

add_subdirectory(src)

add_library(
    base
    STATIC
    ${PATH_LIB}/alu/gate_scalar.c
    ${PATH_LIB}/Gates.c
    ${PATH_LIB}/Memory.c
)

add_compile_options(-lpthread -g -O0)



target_link_libraries(
    silo
    base
    pthread
)
