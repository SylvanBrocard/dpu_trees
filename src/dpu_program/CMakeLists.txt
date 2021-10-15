cmake_minimum_required(VERSION 3.15...3.19)

project(dpu_program)

# set(CMAKE_SYSTEM_NAME Generic) set(CMAKE_SYSTEM_PROCESSOR dpu)

if(NOT DEFINED UPMEM_HOME)
  if("$ENV{UPMEM_HOME}" STREQUAL "")
    set(UPMEM_HOME "/usr")
  else()
    set(UPMEM_HOME $ENV{UPMEM_HOME})
  endif()
endif()

include("${UPMEM_HOME}/share/upmem/cmake/dpu.cmake")

# set(CMAKE_C_COMPILER ${tools}/dpu-upmem-dpurte-clang) set(CMAKE_ASM_COMPILER
# ${tools}/dpu-upmem-dpurte-clang)

# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# set(CMAKE_C_COMPILER_FORCED 1)

add_executable(helloworld helloworld.c)
add_executable(trivial_checksum_example trivial_checksum_example.c)

install(TARGETS helloworld DESTINATION .)
install(TARGETS trivial_checksum_example DESTINATION .)
