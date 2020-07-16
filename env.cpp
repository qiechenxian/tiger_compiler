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

    /// 这是为了内置函数的形参(int[])提供类型
    S_enter(t, S_Symbol((char*)"$int"), TY_Array(TY_Int()));
    return t;
}

S_table E_base_valueEntry(S_table tenv){
    S_table t = S_empty();

    /** 内置函数 */
    E_envEntry funEntry;
    auto array1d = (TY_ty)S_look(tenv, S_Symbol((char*)"$int"));

    /// (1) int getint()
    funEntry = E_FunEntry(nullptr, nullptr, TY_Int());
    S_enter(t, S_Symbol((char*)"getint"), funEntry);

    /// (2) int getch()
    funEntry = E_FunEntry(nullptr, nullptr, TY_Int());
    S_enter(t, S_Symbol((char*)"getch"), funEntry);

    /// (3) int getarray(int [])
    funEntry = E_FunEntry(nullptr, TY_TyList(array1d, nullptr), TY_Int());
    S_enter(t, S_Symbol((char*)"getarray"), funEntry);

    /// (4) void putint(int)
    funEntry = E_FunEntry(nullptr, TY_TyList(TY_Int(), nullptr), TY_Void());
    S_enter(t, S_Symbol((char*)"putint"), funEntry);

    /// (5) void putch(int)
    funEntry = E_FunEntry(nullptr, TY_TyList(TY_Int(), nullptr), TY_Void());
    S_enter(t, S_Symbol((char*)"putch"), funEntry);

    /// (6) void putarray(int, int[])
    TY_tyList formals = TY_TyList(TY_Int(), TY_TyList(array1d, nullptr));
    funEntry = E_FunEntry(nullptr, formals,TY_Void());
    S_enter(t, S_Symbol((char*)"putarray"), funEntry);

    /// (7) void putf()
    funEntry = E_FunEntry(nullptr, nullptr, TY_Void());
    S_enter(t, S_Symbol((char*)"putf"), funEntry);

    /// (8) void starttime()
    funEntry = E_FunEntry(nullptr, nullptr, TY_Void());
    S_enter(t, S_Symbol((char*)"starttime"), funEntry);

    /// (9) void stoptime()
    funEntry = E_FunEntry(nullptr, nullptr, TY_Void());
    S_enter(t, S_Symbol((char*)"stoptime"), funEntry);

    return t;
}


