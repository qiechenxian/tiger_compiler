//
// Created by loyx on 2020/5/1.
//

#ifndef COMPILER_LOYX_SEMANT_H
#define COMPILER_LOYX_SEMANT_H

#include "ast.h"
#include "types.h"
#include "errormsg.h"
#include "translate.h"

typedef void* F_fragList ;

struct expty {Tr_exp exp; TY_ty ty;bool isConst;};
struct expty Expty(Tr_exp exp, TY_ty ty, bool isConst);

F_fragList SEM_transProgram(S_table venv, S_table tenv, A_decList program);

#endif //COMPILER_LOYX_SEMANT_H
