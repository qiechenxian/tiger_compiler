cmake_minimum_required(VERSION 3.15)
project(compiler_dev_cpp_new)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directories(${CMAKE_SOURCE_DIR}/src DIR_SRC) 

add_executable(compiler_dev_cpp_new ${DIR_SRC})

