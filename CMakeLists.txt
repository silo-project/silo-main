cmake_minimum_required(VERSION 3.15)
project(silo C)

set(CMAKE_VERBOSE_MAKEFILE true)
set(PATH_LIB src/lib)

add_subdirectory(src)

add_compile_options(-g -O0 -std=c11)

add_library(
	base
	src/lib/base.c
)

add_library(
	silostd
	src/include/silostdlib.c
)


target_link_libraries(
    silo
    silostd
    base
    pthread
)
