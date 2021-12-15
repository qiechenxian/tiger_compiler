//
// Created by 且趁闲 on 13/8/2020.
//

#ifndef COMPILER_DEV_CPP_NEW_DATA_FLOW_H
#define COMPILER_DEV_CPP_NEW_DATA_FLOW_H
#include "graph.h"
#include "liveness.h"
#include "flowgraph.h"
#include "codegen.h"
#include "temp.h"
AS_instrList dataflow(F_frame f, AS_instrList iList,TAB_table div_table);
#endif //COMPILER_DEV_CPP_NEW_DATA_FLOW_H
