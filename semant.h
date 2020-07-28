//
// Created by loyx on 2020/5/1.
//

#ifndef COMPILER_LOYX_SEMANT_H
#define COMPILER_LOYX_SEMANT_H

#include "ast.h"
#include "types.h"
#include "errormsg.h"
#include "translate.h"


struct expty {
    Tr_exp exp;
    TY_ty ty;
    bool isConst;

    /**
     * suffix_size是翻译数组访问时使用的
     * 来源于transDec中，手动统计suffix信息
     * suffix_size中的-1标志结尾
     */
    int* suffix_size;
    int callee_args;
};
struct expty Expty(Tr_exp exp, TY_ty ty);

F_fragList SEM_transProgram(S_table venv, S_table tenv, A_decList program);

#endif //COMPILER_LOYX_SEMANT_H
