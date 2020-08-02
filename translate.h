//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_TRANSLATE_H
#define COMPILER_LOYX_TRANSLATE_H

#include "frame.h"
#include "ast.h"
#include "array_init.h"
#include <cstdio>
/**
 *
 * 管理栈帧结构与局部变量
 * 解耦 与机器无关的栈帧结构 和 语义检查阶段。将两者关联留在translate模块里
 * 同时支持扩展静态链等
 *
 * */
typedef F_access Tr_access;
typedef F_accessList Tr_accessList;
typedef F_frame Tr_frame;


Tr_frame Tr_newFrame(Temp_label name, U_boolList formals);
Tr_access Tr_allocLocal(Tr_frame frame, bool escape, int size);
Tr_access Tr_allocGlobal(S_symbol global);
Tr_accessList Tr_getFormals(Tr_frame frame);
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);// 构造函数
Temp_label Tr_getGlobalLabel(Tr_access access);


/** 全局变量的frag */
void Tr_newIntFrag(Temp_label label, int value, bool comm);
void Tr_newArrayFrag(Temp_label label, int size, U_pairList values, bool comm);

/**
 *
 * 将AST转换为中间树语言
 *
 * */
typedef struct Tr_exp_ *Tr_exp ;
typedef struct Tr_expList_ *Tr_expList;
typedef struct patchList_ *patchList;
typedef struct Tr_INIT_initList_ *Tr_INIT_initList;
struct Tr_INIT_initList_{
    Tr_exp* array;
    int array_length;
    int* init_offset;
};
/** expList 相关操作 */
Tr_expList Tr_ExpList(); /// return an empty expList
void Tr_expList_append(Tr_expList list, Tr_exp exp);
void Tr_expList_prepend(Tr_expList list, Tr_exp exp);
bool Tr_expList_isEmpty(Tr_expList list);

/** 具体语句转换函数 */
Tr_exp Tr_nopExp();
Tr_exp Tr_intExp(int int_value);
Tr_exp Tr_simpleVar(Tr_access acc);
Tr_exp Tr_simpleVarNoMem(Tr_access acc);
Tr_exp Tr_subscriptVar(Tr_exp base, Tr_exp offset, int dimension);
Tr_exp Tr_subscriptVarNoMem(Tr_exp base, Tr_exp offset, int dimension);
Tr_exp Tr_binop(A_binOp aop,Tr_exp left,Tr_exp right);
Tr_exp Tr_relop(A_binOp aop,Tr_exp left,Tr_exp right);
Tr_exp Tr_if_else(Tr_exp condition_part,Tr_exp then_part,Tr_exp else_part);
Tr_exp Tr_init_array(Tr_access base, Tr_INIT_initList int_info);
Tr_exp Tr_doneExp();
Tr_exp Tr_initialExp();
Tr_exp Tr_break(Tr_exp done);
Tr_exp Tr_continue(Tr_exp initial);
Tr_exp Tr_while(Tr_exp w_cond,Tr_exp w_stmt,Tr_exp w_break,Tr_exp w_continue);
Tr_exp Tr_func_call(Temp_label name,Tr_expList params);
Tr_exp Tr_assign(Tr_exp left,Tr_exp right);
Tr_exp Tr_seq(Tr_exp left,Tr_exp right);
Tr_exp Tr_return(Tr_exp ret_num,F_frame f_frame);
Tr_exp Tr_newlabel();
Tr_frame Tr_root_frame();
F_fragList Tr_getResult();
Tr_exp  Tr_add_fuc_head_label(Tr_exp returnValue,Temp_label fun_label);
Tr_exp Tr_StringExp(c_string content);
void Tr_procEntryExit(Tr_frame frame, Tr_exp body, Tr_accessList formals);
Tr_exp Tr_Nx(T_stm stm);
Tr_exp Tr_Ex_cover(F_access var_access);
#endif //COMPILER_LOYX_TRANSLATE_H
