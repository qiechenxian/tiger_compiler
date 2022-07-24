//
// Created by 13948 on 2020/7/27.
//

#ifndef COMPILER_BUG_CONSTEXP_COLOR_H
#define COMPILER_BUG_CONSTEXP_COLOR_H

#include "temp.h"
#include "assem.h"
#include "graph.h"
#include "liveness.h"

struct COL_result {
    Temp_map coloring;
    Temp_tempList colored;
    Temp_tempList spills;
    AS_instrList coalescedMoves;
    Temp_tempList coalescedNodes;
    G_table alias;
    ~COL_result(){
//        if(coloring!=NULL){
//            delete []coloring;
//            coloring = NULL;
//        }
//        if(colored!=NULL){
//            delete []colored;
//            colored = NULL;
//        }
//        if(spills!=NULL){
//            delete []spills;
//            spills = NULL;
//        }
//        if(coalescedMoves!=NULL){
//            delete []coalescedMoves;
//            coalescedMoves = NULL;
//        }
//        if(coalescedNodes!=NULL){
//            delete []coalescedNodes;
//            coalescedNodes = NULL;
//        }
//        if(alias!=NULL) {
//            delete []alias;
//            alias = NULL;
//        }
    }
};

// 冲突图 ; 预着色 ; 寄存器列表;
// 有可能合并的传送指令集和;节点映射;临时变量的定值次数
struct COL_result COL_color(G_graph ig, Temp_map precolored_map, Temp_tempList regs, Temp_tempList initial,
                            AS_instrList worklistMoves, Temp_map moveList, Temp_map spillCost);

G_node temp2Node(Temp_temp t);

bool precolred(Temp_temp u);
bool precolred(G_node nu);

long degree(Temp_temp t);
long degree(G_node n);

long degreeAdd(G_node nu, long num);
long degreeAdd(Temp_temp u, long num);

void COL_init_spilled_temp_num(int num);

////color和regalloc重复函数
//static Temp_tempList instDef(AS_instr inst);
//static Temp_tempList instUse(AS_instr inst);
#endif //COMPILER_BUG_CONSTEXP_COLOR_H
