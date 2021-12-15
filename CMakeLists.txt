cmake_minimum_required(VERSION 3.15)
project(compiler_dev_cpp_new)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(compiler_dev_cpp_new
        ast_visitor.cpp
        ast_visitor.h
        arm_frame.cpp
        array_init.cpp
        array_init.h
        assem.cpp
        assem.h
        ast.cpp
        ast.h
        cannon.cpp
        cannon.h
        codegen.cpp
        codegen.h
        color.cpp
        color.h
        data_flow.h
        data_flow.cpp
        env.cpp
        env.h
        errormsg.cpp
        errormsg.h
        escape.cpp
        escape.h
        flowgraph.cpp
        flowgraph.h
        frame.h
        graph.h
        graph.cpp

        lex.yy.cpp
        liveness.cpp
        liveness.h
        loop_trans.cpp
        loop_trans.h
        main.cpp
        prast.cpp
        prast.h
        print_ir_tree.h
        printf_ir_tree.cpp
        regalloc.cpp
        regalloc.h
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
        util.h)
