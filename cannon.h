//
// Created by Administrator on 11/7/2020.
//
#include "temp.h"
#include "tree.h"
#include "stdio.h"
#ifndef COMPILER_BUG_CONSTEXP_CANNON_H
#define COMPILER_BUG_CONSTEXP_CANNON_H
typedef struct C_stmListList_ *C_stmListList;
struct C_block{C_stmListList stmLists;Temp_label labels;};
struct C_stmListList_{T_stmList head;C_stmListList tail;};
C_stmListList C_StmListList(T_stmList head,C_stmListList tail);
T_stmList C_linearize(T_stm stm);
struct C_block C_basicBlocks(T_stmList stmList);
T_stmList  C_traceSchedule(struct C_block b);
C_stmListList block_trans(T_stmList stm,Temp_label temp_done);
#endif //COMPILER_BUG_CONSTEXP_CANNON_H
