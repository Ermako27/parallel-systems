cmake_minimum_required(VERSION 3.16)
project(lab01 C)

include_directories(/usr/local/Cellar/mpich/4.0_1/include/)

set(CMAKE_C_STANDARD 99)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(lab01 main.c matrix.c tree.c matrix.h tree.h)