cmake_minimum_required(VERSION 3.15...3.19)

# set(CMAKE_C_COMPILER "gcc") set(CMAKE_CXX_COMPILER "g++")

project(dpu_trees VERSION "0.0.1")

if(SKBUILD)
  # Scikit-Build does not add your site-packages to the search path
  # automatically, so we need to add it _or_ the pybind11 specific directory
  # here.
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_compile_options("`dpu-pkg-config --cflags --libs dpu`")
# add_definitions("`dpu-pkg-config --cflags dpu`")
# add_definitions("-I/usr/include/dpu")

# Now we can find pybind11
find_package(pybind11 CONFIG REQUIRED)
# find_package(dpu CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DPU REQUIRED dpu)

pybind11_add_module(_core MODULE src/main.cpp src/trees.c)
# target_include_directories(_core PRIVATE /usr/include/dpu)
# target_link_libraries(_core PRIVATE dpu)
target_link_libraries(_core PRIVATE ${DPU_LIBRARIES})
target_include_directories(_core PRIVATE ${DPU_INCLUDE_DIRS})
target_compile_options(_core PRIVATE ${DPU_CFLAGS_OTHER})

target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

install(TARGETS _core DESTINATION .)
