//
// Created by zcjsh on 2020/7/26.
//

#ifndef COMPILER_FLOWGRAPH_H
#define COMPILER_FLOWGRAPH_H

#include "temp.h"
#include "frame.h"
#include "graph.h"
#include "assem.h"
Temp_tempList FG_def(G_node n);
Temp_tempList FG_use(G_node n);
bool FG_isMove(G_node n);
AS_instr FG_inst(G_node n);
G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f);
#endif //COMPILER_FLOWGRAPH_H
