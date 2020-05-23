//
// Created by loyx on 2020/4/29.
//

#ifndef COMPILER_LOYX_ABSYN_H
#define COMPILER_LOYX_ABSYN_H

#include "symbol.h"

typedef struct A_pos_* A_pos;

typedef struct A_dec_ *A_dec;
typedef struct A_var_ *A_var;
typedef struct A_stm_ *A_stm;
typedef struct A_exp_ *A_exp;
typedef struct A_ty_ *A_ty;
typedef struct A_field_ *A_field;
typedef struct A_fieldList_ *A_fieldList;
typedef struct A_decList_* A_decList;
typedef struct A_expList_* A_expList;
typedef struct A_comStm_ *A_comStm;
typedef struct A_comStmList_* A_comStmList;
typedef struct A_case_* A_case;
typedef struct A_caseList_* A_caseList;
typedef struct A_initNode_ *A_initNode;
typedef struct A_arrayInitList_ *A_arrayInitList;

typedef enum {
    A_or,
    A_and,
    A_not,
    A_lt, A_le, A_gt, A_ge, A_eq, A_ne,
    A_add, A_sub,
    A_mul, A_div,
    A_mod
}A_binOp;

/** 辅助函数 */
A_decList A_conform(A_decList decs, S_symbol type);
A_decList A_castDecList(A_decList list1, A_decList list2);

/** program */
struct A_pos_ {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
A_pos A_Pos(int a, int b, int c, int d);


struct A_dec_{
    A_pos pos;
    enum {A_variableDec, A_arrayDec, A_functionDec, A_typedef}kind;
    /// todo typedef struct
    union {
        struct {
            S_symbol type;
            S_symbol id;
            A_exp init; /// init is optional
            bool isConst;
            bool escape;
        }var;
        struct {
            S_symbol base;
            S_symbol id;
            A_expList size; /// only intExp
            A_arrayInitList init; /// init is optional
            bool isConst;
            bool escape;
        }array;
        struct {
            S_symbol id;
            A_fieldList params;
            S_symbol result;
            A_stm body;
        }function;
    }u;
};
A_dec A_VariableDec(A_pos pos, S_symbol type, S_symbol id, A_exp init, bool isConst);
A_dec A_ArrayDec(A_pos pos, S_symbol base, S_symbol id, A_expList size, A_arrayInitList init, bool isConst);
A_dec A_FunctionDec(A_pos pos, S_symbol id, A_fieldList params, S_symbol result, A_stm body);


struct A_var_{
    A_pos pos;
    enum {A_simpleVar, A_arrayVar, A_structVar}kind;
    union {
        S_symbol simple;
        struct {
            A_var id;
            A_exp index;
        }arrayVar;
        struct {
            /// todo struct var
            A_var id;
            S_symbol attr;
        }structVar;
    }u;
};
A_var A_SimpleVar(A_pos pos, S_symbol simple);
A_var A_ArrayVar(A_pos pos, A_var id, A_exp index);


struct A_stm_ {
    A_pos pos;
    enum {
        A_ifStm,
        A_whileStm,
        A_blockStm,
        A_assignStm,
        A_switchStm,
        A_returnStm,
        A_breakStm,
        A_continue,
        A_expStm
    }kind;
    union {
        struct {
            A_exp test;
            A_stm body, elseBody;
            /// elseBody is optional
        }ifStm;
        struct {
            A_exp test;
            A_stm body;
        }whileStm;
        A_comStmList blockStm; /// it can be null
        struct {
            A_var var;
            A_exp exp;
        }assignStm;
        struct {
            A_exp exp;
            A_caseList body;
        }switchStm;
        A_exp returnStm;
        /** break stm only need pos*/
        /** continue stm only need pos*/
        A_exp expStm;
    }u;
};
A_stm A_IfStm(A_pos pos, A_exp test, A_stm body, A_stm elseBody);
A_stm A_WhileStm(A_pos pos, A_exp test, A_stm body);
A_stm A_BlockStm(A_pos pos, A_comStmList blockStm);
A_stm A_AssignStm(A_pos pos, A_var var, A_exp exp);
A_stm A_SwitchStm(A_pos pos, A_exp exp, A_caseList body);
A_stm A_ReturnStm(A_pos pos, A_exp returnStm);
A_stm A_BreakStm(A_pos pos);
A_stm A_ContinueStm(A_pos pos);
A_stm A_ExpStm(A_pos pos, A_exp exp);

struct A_exp_{
    A_pos pos;
    enum {
        A_varExp,
        A_intExp,
        A_charExp,
        A_callExp,
        A_opExp,
        A_logicExp
    }kind;
    union {
        A_var varExp;
        int intExp;
        char charExp;
        struct {
            S_symbol id;
            A_expList args;
        }callExp;
        struct {
            A_exp left;
            A_binOp op;
            A_exp right;
        }opExp;
        struct {
            A_binOp op;
            A_exp factor1;
            A_exp factor2; /// this is optional
        }logicExp;
    }u;
};
A_exp A_VarExp(A_pos pos, A_var varExp);
A_exp A_IntExp(A_pos pos, int intExp);
A_exp A_CharExp(A_pos pos, char charExp);
A_exp A_CallExp(A_pos pos, S_symbol id, A_expList args);
A_exp A_OpExp(A_pos pos, A_exp left, A_binOp op, A_exp right);
A_exp A_LogicExp(A_pos pos, A_binOp op, A_exp factor1, A_exp factor2);

/** 支持扩展 typedef */
struct A_ty_{
    A_pos pos;
    enum {A_aliasTy, A_arrayTy, A_structTy}kind;
    union {
        S_symbol name;
        A_fieldList structTy;
    }u;
};


/** Linked lists and nodes of lists */

struct A_initNode_{
    A_pos pos;
    enum {A_singleInit, A_nestedInit}kind;
    union {
        A_exp single;
        A_arrayInitList nested;
    }u;
};
A_initNode A_SingleInit(A_pos pos, A_exp single);
A_initNode A_NestedInit(A_pos pos, A_arrayInitList nested);

struct A_arrayInitList_{
    A_initNode head;
    A_arrayInitList tail;
};
A_arrayInitList A_ArrayInitList(A_initNode head, A_arrayInitList tail);

struct A_field_{
    A_pos pos;
    bool isConst; /// 可扩展const形参
    S_symbol id;
    S_symbol type;
    A_expList size;
    bool escape;
};
A_field A_Field(A_pos pos, S_symbol id, S_symbol type, A_expList size);

struct A_fieldList_{
    A_field head;
    A_fieldList tail;
};
A_fieldList A_FieldList(A_field head, A_fieldList tail);

struct A_decList_{
    A_dec head;
    A_decList tail;
};
A_decList A_DecList(A_dec head, A_decList tail);

struct A_expList_{
    A_exp head;
    A_expList tail;
};
A_expList A_ExpList(A_exp head, A_expList tail);

struct A_comStm_{
    /// const_var_decStm is optional
    A_decList const_var_decStm;/// not function declare
    A_stm stmSeq;
};
A_comStm A_ComStm(A_decList const_var_decStm, A_stm stmSeq);

struct A_comStmList_{
    A_comStm head;
    A_comStmList tail;
};
A_comStmList A_ComStmList(A_comStm head, A_comStmList tail);

struct A_case_{
    int const_value;
    A_stm body;
};
A_case A_Case(int const_value, A_stm body);

struct A_caseList_{
    A_case head;
    A_caseList tail;
};
A_caseList A_CaseList(A_case head, A_caseList tail);


#endif //COMPILER_LOYX_ABSYN_H
