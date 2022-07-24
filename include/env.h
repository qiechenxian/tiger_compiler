//
// Created by loyx on 2020/4/27.
//

#ifndef COMPILER_LOYX_ENV_H
#define COMPILER_LOYX_ENV_H

#include "types.h"
#include "translate.h"
#include "temp.h"
#include "array_init.h"

typedef struct E_envEntry_ *E_envEntry;
typedef struct E_constValues_ *E_constValues;

struct E_envEntry_{
    enum {E_varEntry, E_funEntry}kind;
    union {
        struct {
            bool isConst;
            E_constValues cValues;
            Tr_access access;
            TY_ty ty;

            /**
             * 为翻译数组访问，需要在符号表中记录数组维度后缀和
             * 约定后缀和数组以-1结尾。例如:[13, 3, 1, -1]
             */
            int* suffix_size;
            bool isFormal;
        }var;
        struct {Temp_label label; TY_tyList formals; TY_ty result;}fun;
    }u;
};

struct E_constValues_{
    /**
     * E_constValues 为符号表中 VarEntry 保存 const 变量的值的数据结构
     *      1. singleValue保存const int foo = ...;
     *          1.1. singleValue为int
     *      2. arrayValue保存const int foo[bar] = {...}
     *          2.1. arrayValue为int数组
     *          2.2. arrayValue需要将初始值(e.g.{1,{2,},3,4})转化为int数组，此处涉及数组初始值的处理，暂未实现
     *          2.3. arrayValue配有相应的查值函数， 在语义检查阶段辅助transVar工作， 将const变量引用变为数值
     *      3. 未考虑free
     *
     * --loyx 2020/6/24
     *
     * 变更：
     * 1. arrayValue的类型更改为INIT_initList(详情array_init.[h|cpp])
     * 2. arrayValue的处理在INIT_initList的构造函数中完成
     * 3. arrayValue的查值函数由array_init模块提供(INIT_getInt)
     * --loyx 2020/7/13
     *
     */
    enum {E_singleValue, E_arrayValue}kind;
    union {
        int singleValue;
        INIT_initList arrayValue;
    }u;
};

E_envEntry E_VarEntry(bool isConst, Tr_access access, TY_ty ty);
E_envEntry E_FunEntry(Temp_label label, TY_tyList formals, TY_ty result);

E_constValues E_SingleValue(int v);
E_constValues E_ArrayValue(INIT_initList vs);

S_table E_base_typeEntry();
S_table E_base_valueEntry(S_table tenv);

#endif //COMPILER_LOYX_ENV_H
