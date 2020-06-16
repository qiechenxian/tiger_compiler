#文件说明

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
可能得移动到语义阶段。

##更新
增加了栈帧结构的层次level封装，方便嵌套访问外层变量时通过静态链进行变量的level回溯访问
添加frame的locals局部变量区域，formals视为参数区域且默认formals顶部指针指向静态链存储区
暂时默认所有变量存储在frame中，寄存器分配待补充
目前中间代码生成还在写，暂时只完成了simplevar的翻译
我的ubuntu系统当时改python版本时的出了些问题，暂时是在windows下修改代码