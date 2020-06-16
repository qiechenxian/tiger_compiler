//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_TRANSLATE_H
#define COMPILER_LOYX_TRANSLATE_H
#include "tree.h"
#include "frame.h"

/**
 *
 * 管理栈帧结构与局部变量
 * 解耦 与机器无关的栈帧结构 和 语义检查阶段。将两者关联留在translate模块里
 * 同时支持扩展静态链等
 *
 * */
typedef Tr_access_* Tr_access;
typedef F_accessList Tr_accessList;
typedef F_frame Tr_frame;
typedef Tr_level_* Tr_level;
Tr_frame Tr_newFrame(Temp_label name, U_boolList formals);
Tr_access Tr_allocLocal(Tr_frame frame, bool escape);
Tr_accessList Tr_getFormals(Tr_frame frame);
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);// 构造函数


/**
 *
 * 将AST转换为中间树语言
 *
 * */
typedef struct Tr_exp_ *Tr_exp ;
typedef struct Tr_expList_ *Tr_expList;
typedef struct patchList_ *patchList;

/** expList 相关操作 */
Tr_expList Tr_ExpList(); /// return an empty expList
void Tr_expList_append(Tr_expList list, Tr_exp exp);
void Tr_expList_prepend(Tr_expList list, Tr_exp exp);
bool Tr_expList_isEmpty(Tr_expList list);

/** 具体语句转换函数 */

#endif //COMPILER_LOYX_TRANSLATE_H
