cmake_minimum_required(VERSION 3.15...3.19)

project(dpu_program)

set(CMAKE_C_COMPILER "dpu-upmem-dpurte-clang")

add_executable(helloworld helloworld.c)
add_executable(trivial_checksum_example trivial_checksum_example.c)

install(TARGETS helloworld DESTINATION .)
install(TARGETS trivial_checksum_example DESTINATION .)
