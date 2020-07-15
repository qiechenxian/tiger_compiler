//
// Created by loyx on 2020/4/25.
//

#ifndef COMPILER_LOYX_SYMBOL_H
#define COMPILER_LOYX_SYMBOL_H
#include "util.h"

typedef struct S_symbol_ *S_symbol;
S_symbol S_Symbol(c_string);
c_string S_getName(S_symbol);

typedef struct TAB_tabel_* S_table;
S_table S_empty();
void S_enter(S_table t, S_symbol sym, void* value);
void* S_look(S_table t, S_symbol sym);
void S_beginScope(S_table t);
void S_endScope(S_table t);

#endif //COMPILER_LOYX_SYMBOL_H
