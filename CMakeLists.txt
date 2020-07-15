cmake_minimum_required(VERSION 3.16)
project(compiler_bug_constexp)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(compiler_bug_constexp
        arm_frame.cpp
        ast.cpp
        ast.h
        env.cpp
        env.h
        errormsg.cpp
        errormsg.h
        escape.cpp
        escape.h
        frame.h
        lex.yy.cpp
        main.cpp
        semant.cpp
        semant.h
        symbol.cpp
        symbol.h
        SysY.tab.cpp
        SysY.tab.h
        table.cpp
        table.h
        temp.cpp
        temp.h
        translate.cpp
        translate.h
        tree.cpp
        tree.h
        types.cpp
        types.h
        util.cpp
        util.h cannon.h cannon.cpp printtree.c printtree.h codegen.h assem.h arm_codegen.c assem.cpp)
