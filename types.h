//
// Created by loyx on 2020/4/25.
//

#ifndef COMPILER_LOYX_TYPES_H
#define COMPILER_LOYX_TYPES_H
#include "symbol.h"

typedef struct TY_ty_* TY_ty;
typedef struct TY_tyList_* TY_tyList;
typedef struct TY_field_* TY_field;
typedef struct TY_fieldList_* TY_fieldList;

struct TY_ty_{
    enum {
        TY_int,
        TY_char,
        TY_void,

        TY_array,
        TY_struct,
        TY_typedef
    }kind;
    union {
        TY_ty array;
        TY_fieldList ty_struct;
        struct {S_symbol sym; TY_ty ty;}ty_typedef;
    }u;
};
TY_ty TY_Int();
TY_ty TY_Char();
TY_ty TY_Void();

TY_ty TY_Array(TY_ty ty);
TY_ty TY_Struct(TY_fieldList fields);
TY_ty TY_Typedef(S_symbol sym, TY_ty ty);


struct TY_tyList_{
    TY_ty head;
    TY_tyList tail;
};
TY_tyList TY_TyList(TY_ty ty, TY_tyList tail);


struct TY_field_{
    S_symbol sym;
    TY_ty ty;
};
TY_field TY_Field(S_symbol sym, TY_ty ty);


struct TY_fieldList_{
    TY_field head;
    TY_fieldList tail;
};
TY_fieldList TY_FieldList(TY_field head, TY_fieldList tail);

string TY_toString(TY_ty ty);


#endif //COMPILER_LOYX_TYPES_H
