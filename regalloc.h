//
// Created by 13948 on 2020/7/27.
//

#ifndef COMPILER_BUG_CONSTEXP_REGALLOC_H
#define COMPILER_BUG_CONSTEXP_REGALLOC_H

/*
 * 参考链接
 * https://github.com/shihaoL/Tiger-Compiler/blob/master/src/regalloc.c
*/
#include "color.h"

struct RA_result {
    Temp_map coloring;
    AS_instrList il;
};

struct RA_result RA_regAlloc(F_frame f, AS_instrList il);


#endif //COMPILER_BUG_CONSTEXP_REGALLOC_H
