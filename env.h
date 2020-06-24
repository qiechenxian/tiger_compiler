//
// Created by loyx on 2020/4/27.
//

#ifndef COMPILER_LOYX_ENV_H
#define COMPILER_LOYX_ENV_H

#include "types.h"
#include "translate.h"
#include "temp.h"

typedef struct E_envEntry_ *E_envEntry;

struct E_envEntry_{
    enum {E_varEntry, E_funEntry}kind;
    union {
        struct {bool isConst; Tr_access access; TY_ty ty;}var;
//        struct {bool isConst; E_constValues cValues; Tr_access access; TY_ty ty;}var; todo ConstExp
        struct {Temp_label label; TY_tyList formals; TY_ty result;}fun;
    }u;
};

E_envEntry E_VarEntry(bool isConst, Tr_access access, TY_ty ty);
E_envEntry E_FunEntry(Temp_label label, TY_tyList formals, TY_ty result);

S_table E_base_typeEntry();
S_table E_base_valueEntry();

#endif //COMPILER_LOYX_ENV_H
