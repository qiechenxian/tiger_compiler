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

typedef struct tag_temp_* tag_temp;//temp中int记载合并头标号
struct tag_temp_{
    int tag_number;
    Temp_temp temp;
};

typedef struct tag_list_* tag_list;//总数链表
struct tag_list_{
    int number;
    tag_temp head;
    tag_list tail;
};
#endif //COMPILER_BUG_CONSTEXP_REGALLOC_H
