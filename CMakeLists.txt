cmake_minimum_required(VERSION 3.16)
project(codigo_c C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(codigo_c
        gen_matrix.c
        qr_givens.c
        qr_givens.h
        )

target_link_libraries(codigo_c PRIVATE m)