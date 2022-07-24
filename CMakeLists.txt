cmake_minimum_required(VERSION 3.15)
project(compiler_dev_cpp_new)
set(CMAKE_CXX_STANDARD 14)


include_directories(./include)

set(ASMJIT_STATIC TRUE)
add_subdirectory(./asmjit)

file(GLOB_RECURSE COMPILE_SRC ./src/*.cpp)
add_executable(compiler_dev_cpp_new ${COMPILE_SRC})
target_link_libraries(compiler_dev_cpp_new asmjit::asmjit) 


