//
// Created by loyx on 2020/4/27.
//

#include "env.h"

E_envEntry E_VarEntry(bool isConst, Tr_access access, TY_ty ty){
    E_envEntry e = checked_malloc(sizeof(*e));
    e->kind = E_varEntry;
    e->u.var.isConst = isConst;
    e->u.var.access = access;
    e->u.var.ty = ty;
    return e;
}
E_envEntry E_FunEntry(Temp_label label, TY_tyList formals, TY_ty result){
    E_envEntry e = checked_malloc(sizeof(*e));
    e->kind = E_funEntry;
    e->u.fun.label = label;
    e->u.fun.formals = formals;
    e->u.fun.result = result;
    return e;
}

S_table E_base_typeEntry(){
    S_table t = S_empty();
    S_enter(t, S_Symbol("int"), TY_Int());
    S_enter(t, S_Symbol("void"), TY_Void());
    return t;
}
S_table E_base_valueEntry(){
    S_table t = S_empty();
    /// build in functions
    return t;
}
