//
// Created by 13948 on 2020/7/27.
//

#ifndef COMPILER_BUG_CONSTEXP_COLOR_H
#define COMPILER_BUG_CONSTEXP_COLOR_H

/*
 * 参考链接
 * https://github.com/shihaoL/Tiger-Compiler/blob/master/src/color.c
*/
struct COL_result {Temp_map coloring; Temp_tempList spills;};
struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs); //冲突图 ; 预着色 ; 寄存器列表
#endif //COMPILER_BUG_CONSTEXP_COLOR_H
