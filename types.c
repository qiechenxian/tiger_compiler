//
// Created by loyx on 2020/4/25.
//

#include "types.h"


static struct TY_ty_ ty_int = {TY_int};
TY_ty TY_Int(void) {return &ty_int;}

static struct TY_ty_ ty_char = {TY_char};
TY_ty TY_Char(void) {return &ty_char;}

static struct TY_ty_ ty_void = {TY_void};
TY_ty TY_Void(void) {return &ty_void;}

TY_ty TY_Array(TY_ty ty){
    TY_ty p = checked_malloc(sizeof(*p));
    p->kind = TY_array;
    p->u.array = ty;
    return p;
}

TY_ty TY_Struct(TY_fieldList fields){
    TY_ty p = checked_malloc(sizeof(*p));
    p->kind = TY_struct;
    p->u.ty_struct = fields;
    return p;
}

TY_ty TY_Typedef(S_symbol sym, TY_ty ty){
    TY_ty p = checked_malloc(sizeof(*p));
    p->kind = TY_typedef;
    p->u.ty_typedef.sym = sym;
    p->u.ty_typedef.ty = ty;
    return p;
}

TY_tyList TY_TyList(TY_ty ty, TY_tyList tail){
    TY_tyList p = checked_malloc(sizeof(*p));
    p->head = ty;
    p->tail = tail;
    return p;
}


TY_field TY_Field(S_symbol sym, TY_ty ty){
    TY_field p = checked_malloc(sizeof(*p));
    p->sym = sym;
    p->ty = ty;
    return p;
}


TY_fieldList TY_FieldList(TY_field head, TY_fieldList tail){
    TY_fieldList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}


static char error_str_ty[][12] = {
        "int", "char", "void",
        "array", "struct", "typedef"
};

string TY_toString(TY_ty ty){
    if(ty->kind == TY_typedef)
        return S_getName(ty->u.ty_typedef.sym);
    return error_str_ty[ty->kind];
}
