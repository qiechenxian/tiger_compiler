//
// Created by loyx on 2020/8/10.
//

#ifndef COMPILER_2020_LOOP_TRANS_H
#define COMPILER_2020_LOOP_TRANS_H
static const int unroll_new_variable_name_size = 40;

#include "ast.h"

bool LOOP_canUnrolled(A_exp condition);
void LOOP_unrollWhile(A_stm stm);
void LOOP_hoistArrayAccess(A_stm stm, S_table venv);

#endif //COMPILER_2020_LOOP_TRANS_H
