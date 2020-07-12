//
// Created by loyx on 2020/4/27.
//

#include "env.h"

E_envEntry E_VarEntry(bool isConst, Tr_access access, TY_ty ty){
    auto e = (E_envEntry)checked_malloc(sizeof(E_envEntry_));
    e->kind = E_envEntry_::E_varEntry;
    e->u.var.isConst = isConst;
    e->u.var.cValues = nullptr;
    e->u.var.access = access;
    e->u.var.ty = ty;
    return e;
}
E_envEntry E_FunEntry(Temp_label label, TY_tyList formals, TY_ty result){
    auto e = (E_envEntry)checked_malloc(sizeof(E_envEntry_));
    e->kind = E_envEntry_::E_funEntry;
    e->u.fun.label = label;
    e->u.fun.formals = formals;
    e->u.fun.result = result;
    return e;
}

E_constValues E_SingleValue(int v){
    auto e = (E_constValues)checked_malloc(sizeof(E_constValues_));
    e->kind = E_constValues_::E_singleValue;
    e->u.singleValue = v;
    return e;
}
E_constValues E_ArrayValue(INIT_initList vs){
    auto e = (E_constValues)checked_malloc(sizeof(E_constValues_));
    e->kind = E_constValues_::E_arrayValue;
    e->u.arrayValue = vs;
    return e;
}

S_table E_base_typeEntry(){
    auto t = (S_table)S_empty();
    S_enter(t, S_Symbol((char*)"int"), TY_Int());
    S_enter(t, S_Symbol((char*)"void"), TY_Void());
    return t;
}
S_table E_base_valueEntry(){
    S_table t = S_empty();
    /// build in functions
    return t;
}
