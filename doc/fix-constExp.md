# 对constExp问题的修复

## 文法部分：SysY.y
将constExp看作Exp，不做特殊处理

## 语法部分
### 1. env.h(cpp)
在符号表变量表目(E_VarEntry)中添加记录常量初值的
数据结构(E_constValues)。具体细节查看env.h
中的注释。env.cpp中添加其构造函数。
### 2. semant.cpp
constExp主要出现在两个地方：
1. 数组声明中的下标
2. const变量的初始值

由于在文法阶段，将constExp和Exp不做区分。
因此这里的处理方式是：transExp函数
(因为下标和初值都是A_exp类)保证将cosntExp表达式
转化为 intExp，并且在AST上做就地修改。这样，便可
区分constExp和Exp。

相关语法检查和处理集中在transDec函数中。

对用const数组的处理是：1. 将其值以数组形式存放在
符号表中，供之后优化使用。2. 由于对const数组的访问
可能会发生在运行时，因此还应将其当做普通数组处理(可优化)

transExp部分涉及const变量问题，暂未实现。

E_constValues的一些辅助函数涉及数组初值处理，暂未实现。

### 3. ast.h
添加了一些理解性注释。

#其他
修复了一些c++ warning