cmake_minimum_required(VERSION 3.10)
project(compiler_2020)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        compiler_2020

        frame.h
        arm_frame.cpp
        ast.cpp
        ast.h
        env.h
        env.cpp
        errormsg.h
        errormsg.cpp
        escape.cpp
        escape.h
        lex.yy.cpp
        semant.h
        semant.cpp
        symbol.h
        symbol.cpp
        SysY.tab.h
        SysY.tab.cpp
        table.h
        table.cpp
        temp.h
        temp.cpp
        translate.h
        translate.cpp
        tree.h
        tree.cpp
        types.h
        types.cpp
        util.h
        util.cpp
        prast.cpp
        prast.h

        main.cpp
        array_init.cpp array_init.h
        cannon.cpp cannon.h
        print_ir_tree.h
        printf_ir_tree.cpp
        assem.cpp assem.h
        codegen.cpp codegen.h
        graph.cpp graph.h
        flowgraph.cpp flowgraph.h
        color.cpp color.h
        liveness.h liveness.cpp
        regalloc.cpp regalloc.h
)