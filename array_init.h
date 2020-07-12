//
// Created by loyx on 2020/7/12.
//

#ifndef COMPILER_2020_ARRAY_INIT_H
#define COMPILER_2020_ARRAY_INIT_H

#include "ast.h"

typedef struct INIT_initList_ *INIT_initList;
struct INIT_initList_{
    A_exp* array;
    int* suffix_size;
};
INIT_initList INIT_InitList(A_expList array_size, A_arrayInitList array_init_list);

A_exp INIT_get(INIT_initList init_list, const int* index);
int INIT_getInt(INIT_initList init_list, int* index);


#endif //COMPILER_2020_ARRAY_INIT_H
