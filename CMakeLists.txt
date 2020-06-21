cmake_minimum_required(VERSION 3.16)
project(changeToCpp2)

set(CMAKE_CXX_STANDARD 11)

add_executable(changeToCpp2 util.h util.cpp errormsg.h errormsg.cpp table.h table.cpp symbol.h symbol.cpp ast.h ast.cpp
        types.h types.cpp env.h env.cpp temp.h temp.cpp tree.h tree.cpp frame.h arm_frame.cpp translate.h translate.cpp semant.h semant.cpp
        SysY.tab.h  lex.yy.cpp SysY.tab.cpp  main.cpp)