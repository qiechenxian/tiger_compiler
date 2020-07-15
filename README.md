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

## 问题
+ 第一版文法中constExp只有数值，所以在文法分析阶段
直接运算了constExp的值。但现在constExp包含了常数变量
可能得移动到语义阶段。(严重问题) -- 骆艺轩 2020/6/4
+ 目前文法阶段中的exp，使用的是第一版文法。其特点是
文法中不体现优先级。因此实现时，使用bison指导优先级。
最新版本的文法中的exp将优先级写到了文法中(是否有其他
增加我还没看)。要考虑是否更改当前的exp文法。修改后，
可能导致exp文法变复杂、产生冲突。不修改的话，就需要
验证文法正确性。 --骆艺轩 2020/6/4