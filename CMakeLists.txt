cmake_minimum_required(VERSION 3.15)
project(compiler_dev_cpp_new)

set(CMAKE_CXX_STANDARD 14)

include_directories(./include)
file(GLOB_RECURSE compile_srcs ./src/*.cpp)

add_executable(compiler_dev_cpp_new ${compile_srcs})

