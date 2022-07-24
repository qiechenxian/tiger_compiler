//
// Created by 且趁闲 on 22/7/2020.
//

#ifndef COMPILER_DEV_CPP_PRINT_IR_TREE_H
#define COMPILER_DEV_CPP_PRINT_IR_TREE_H
#include "tree.h"
#include "temp.h"
#include "frame.h"
void printStmList (FILE *out,F_fragList stmList);
void printcannoList (FILE *out, T_stmList stmList);
#endif //COMPILER_DEV_CPP_PRINT_IR_TREE_H
