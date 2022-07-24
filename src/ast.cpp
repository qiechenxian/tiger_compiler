//
// Created by loyx on 2020/4/29.
//

#include "ast.h"
#include <string.h>


/** 辅助函数 */

A_decList A_conform(A_decList decs, S_symbol type){
    /**
     * 设定 type id1,id2, ... idn; 语句中每个变量的类型为type
     * 向语法妥协
     * */
    A_decList iter;
    for(iter = decs; iter; iter = iter->tail){
        A_dec dec = iter->head;
        if(dec->kind == A_dec_::A_arrayDec){
            dec->u.array.base = type;
        } else{
            dec->u.var.type = type;
        }
    }
    return decs;
}

A_decList A_castDecList(A_decList list1, A_decList list2){
    /**
     * 将两个A_decList 连接起来
     * */
    A_decList iter = list1;
    while(iter->tail != nullptr){
        iter = iter->tail;
    }
    iter->tail = list2;
    return list1;
}


/**
 * abstract grammar
 * 构造函数
 * */
A_pos A_Pos(int a, int b, int c, int d){
    A_pos pos = (A_pos)checked_malloc(sizeof(*pos));
    pos->first_line = a;
    pos->first_column = b;
    pos->last_line = c;
    pos->last_column = d;
    return pos;
}
A_dec A_VariableDec(A_pos pos, S_symbol type, S_symbol id, A_exp init, bool isConst){
    A_dec a = (A_dec)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_dec_::A_variableDec;
    a->u.var.type = type;
    a->u.var.id = id;
    a->u.var.init = init;
    a->u.var.isConst = isConst;
    a->u.var.suffixSize = nullptr; /// 指针类型使用
    a->u.var.escape = true; /// 默认逃逸变量
    return a;
}
A_dec A_ArrayDec(A_pos pos, S_symbol base, S_symbol id, A_expList size, A_arrayInitList init, bool isConst){
    A_dec a = (A_dec)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_dec_::A_arrayDec;
    a->u.array.base = base;
    a->u.array.id = id;
    a->u.array.size = size;
    a->u.array.init = init;
    a->u.array.isConst = isConst;
    a->u.array.escape = true; /// 默认逃逸变量
    return a;
}
A_dec A_FunctionDec(A_pos pos, S_symbol id, A_fieldList params, S_symbol result, A_stm body){
    A_dec a = (A_dec)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_dec_::A_functionDec;
    a->u.function.id = id;
    a->u.function.params = params;
    a->u.function.result = result;
    a->u.function.body = body;
    return a;
}
A_var A_SimpleVar(A_pos pos, S_symbol simple){
    A_var a = (A_var)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_var_::A_simpleVar;
    a->u.simple = simple;
    return a;
}
A_var A_ArrayVar(A_pos pos, A_var id, A_exp index){
    A_var a = (A_var)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind= A_var_::A_arrayVar;
    a->u.arrayVar.id = id;
    a->u.arrayVar.index = index;
    return a;
}
A_stm A_IfStm(A_pos pos, A_exp test, A_stm body, A_stm elseBody){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_ifStm;
    a->u.ifStm.test = test;
    a->u.ifStm.body = body;
    a->u.ifStm.elseBody = elseBody;
    return a;
}
A_stm A_WhileStm(A_pos pos, A_exp test, A_stm body){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_whileStm;
    a->u.whileStm.test = test;
    a->u.whileStm.body = body;
    return a;
}
A_stm A_BlockStm(A_pos pos, A_comStmList blockStm){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_blockStm;
    a->u.blockStm = blockStm;
    return a;
}
A_stm A_AssignStm(A_pos pos, A_var var, A_exp exp){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_assignStm;
    a->u.assignStm.var = var;
    a->u.assignStm.exp = exp;
    return a;
}
A_stm A_SwitchStm(A_pos pos, A_exp exp, A_caseList body){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_switchStm;
    a->u.switchStm.exp = exp;
    a->u.switchStm.body = body;
    return a;
}
A_stm A_ReturnStm(A_pos pos, A_exp returnStm){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_stm_::A_returnStm;
    a->u.returnStm = returnStm;
    return a;
}
A_stm A_BreakStm(A_pos pos){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a->kind = A_stm_::A_breakStm;
    a->pos = pos;
    return a;
}
A_stm A_ContinueStm(A_pos pos){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a->kind = A_stm_::A_continue;
    a->pos = pos;
    return a;
}
A_stm A_ExpStm(A_pos pos, A_exp exp){
    A_stm a = (A_stm)checked_malloc(sizeof(*a));
    a->kind = A_stm_::A_expStm;
    a->u.expStm =exp;
    return a;
}
A_exp A_VarExp(A_pos pos, A_var varExp){
    A_exp a = (A_exp)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_exp_::A_varExp;
    a->u.varExp = varExp;
    return a;
}
A_exp A_IntExp(A_pos pos, int intExp){
    A_exp a = (A_exp)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_exp_::A_intExp;
    a->u.intExp = intExp;
    return a;
}
A_exp A_CharExp(A_pos pos, char charExp){
    A_exp a = (A_exp)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_exp_::A_charExp;
    a->u.charExp = charExp;
    return a;
}
A_exp A_CallExp(A_pos pos, S_symbol id, A_expList args){
    A_exp a = (A_exp)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_exp_::A_callExp;
    a->u.callExp.id = id;
    a->u.callExp.args = args;
    return a;
}
A_exp A_OpExp(A_pos pos, A_exp left, A_binOp op, A_exp right){
    A_exp a = (A_exp)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->kind = A_exp_::A_opExp;
    a->u.opExp.left = left;
    a->u.opExp.op = op;
    a->u.opExp.right = right;
    return a;
}
A_exp A_StringExp(A_pos pos, c_string stringExp){
    auto a = (A_exp)checked_malloc(sizeof(A_exp_));
    a->pos = pos;
    a->kind = A_exp_::A_stringExp;
    a->u.stringExp = String(stringExp);
    return a;
}
A_field A_Field(A_pos pos, S_symbol id, S_symbol type, A_expList size){
    A_field a = (A_field)checked_malloc(sizeof(*a));
    a-> pos = pos;
    a->isConst = false; /// 默认非const
    a-> id = id;
    a-> type = type;
    a->size = size;
    a->escape = true; /// 默认逃逸变量，即所有变量均可放在寄存器中,但对于形参的前四个除外
    return a;
}
A_fieldList A_FieldList(A_field head, A_fieldList tail){
    A_fieldList a = (A_fieldList)checked_malloc(sizeof(*a));
    a-> head = head;
    a-> tail = tail;
    return a;
}
A_decList A_DecList(A_dec head, A_decList tail){
    A_decList a = (A_decList)checked_malloc(sizeof(*a));
    a-> head = head;
    a-> tail = tail;
    return a;
}
A_expList A_ExpList(A_exp head, A_expList tail){
    A_expList a = (A_expList)checked_malloc(sizeof(*a));
    a-> head = head;
    a-> tail = tail;
    return a;
}
A_comStm A_ComStm(A_decList const_var_decStm, A_stm stmSeq){
    A_comStm a = (A_comStm)checked_malloc(sizeof(*a));
    a-> const_var_decStm = const_var_decStm;
    a-> stmSeq = stmSeq;
    return a;
}
A_comStmList A_ComStmList(A_comStm head, A_comStmList tail){
    A_comStmList a = (A_comStmList)checked_malloc(sizeof(*a));
    a-> head = head;
    a-> tail = tail;
    return a;
}
A_case A_Case(int const_value, A_stm body){
    A_case a = (A_case)checked_malloc(sizeof(*a));
    a-> const_value = const_value;
    a-> body = body;
    return a;
}
A_caseList A_CaseList(A_case head, A_caseList tail){
    A_caseList a = (A_caseList)checked_malloc(sizeof(*a));
    a-> head = head;
    a-> tail = tail;
    return a;
}
A_initNode A_SingleInit(A_pos pos, A_exp single){
    A_initNode a = (A_initNode)checked_malloc(sizeof(*a));
    a->pos = pos;
    a->kind = A_initNode_::A_singleInit;
    a->u.single = single;
    return a;
}
A_initNode A_NestedInit(A_pos pos, A_arrayInitList nested){
    A_initNode a = (A_initNode)checked_malloc(sizeof(*a));
    a->pos = pos;
    a->kind = A_initNode_::A_nestedInit;
    a->u.nested = nested;
    return a;
}
A_arrayInitList A_ArrayInitList(A_initNode head, A_arrayInitList tail){
    A_arrayInitList a = (A_arrayInitList)checked_malloc(sizeof(*a));
    a->head = head;
    a->tail = tail;
    return a;
}
