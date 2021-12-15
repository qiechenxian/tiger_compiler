# 项目来历

## 根据虎书实现的 c子集 -> arm汇编 编译器
+ 2020年华为编译系统设计赛，NWPU
+ 因为最终生成为arm汇编，工作后手头没有arm机器，打算做一版jit的运行时
+ 直接用visual studio2022 打开 .sln 文件，运行即可， test.c 为c子集程序， test.s 为生成的arm汇编

## ToDo list

+ asm_jit运行时支持
+ llvm_jit运行时支持

# 文件说明

## 通用模块
+ util.[ch]: 一些通用函数，宏
+ errormsg.[ch]: 错误信息处理

## 词法阶段
+ lexer.l: flex词法定义
+ lex.yy.c: flex自动生成文件

## 文法阶段
+ SysY.y: bison文法定义
+ SysY.tab.c: bison自动生成文件
+ SysY.tab.h: bison自动生成头文件，供flex使用
+ SysY.output: bison编译信息

## 语义阶段
+ absyn.[ch]: 抽象语法树相关
+ semant.[ch]: 语义检查相关
+ symbol.[ch]: 符号表相关
+ table.[ch]: 符号表的底层实现，通用指针散列表
+ types.[ch]: 类型系统
+ env.[ch]: 类型检查使用的环境

## C0的遗留产物
这些是当时为C0写的
+ frame.h: 通用栈帧的抽象表示
+ arm_frame.c: arm结构栈帧
+ tree.[ch]: 树状IR定义
+ temp.[ch]: 临时变量和标号
+ escape.[ch]: 逃逸变量查找
+ translate.[ch]: AST到IR的转化

## 其他
+ main.c: 程序入口
+ test.c: 简单测试程序

