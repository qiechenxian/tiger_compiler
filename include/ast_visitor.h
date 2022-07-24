//
// Created by loyx on 2020/8/14.
//

#ifndef COMPILER_2020_AST_VISITOR_H
#define COMPILER_2020_AST_VISITOR_H
#include "ast.h"

/**************** AST树的访问者模式 ******************
 *
 * visitor接口：
 *  继承visitor接口的子类，需要实现接收4种元素(A_stm, A_exp, A_dec, A_var)的抽象函数，通过ASTV_registerVisitor注册。
 *  抽象函数的返回值(bool)可控制是否继续迭代ast子树，并提过一个参数供返回信息使用
 *
 * AST迭代函数：
 *  1. ASTV_iterDec
 *  2. ASTV_iterExp
 *  3. ASTV_iterStm
 *  4. ASTV_iterVar
 *
 **************************************************/


typedef bool (*visit_fun_stm)(A_stm, void* arg);
typedef bool (*visit_fun_exp)(A_exp, void* arg);
typedef bool (*visit_fun_dec)(A_dec, void* arg);
typedef bool (*visit_fun_var)(A_var, void* arg);
int ASTV_registerVisitors(visit_fun_dec f_dec, visit_fun_exp f_exp,visit_fun_stm f_stm, visit_fun_var f_var);
int ASTV_registerDefaultVisitors();
int ASTV_registerDecVisitor(visit_fun_dec fun);
int ASTV_registerExpVisitor(visit_fun_exp fun);
int ASTV_registerStmVisitor(visit_fun_stm fun);
int ASTV_registerVarVisitor(visit_fun_var fun);

void ASTV_iterDec(A_dec el, int visitor_id, void* arg);
void ASTV_iterExp(A_exp el, int visitor_id, void* arg);
void ASTV_iterStm(A_stm el, int visitor_id, void* arg);
void ASTV_iterVar(A_var el, int visitor_id, void* arg);

#endif //COMPILER_2020_AST_VISITOR_H
