//
// Created by loyx on 2020/3/24.
//

#ifndef COMPILER_LOYX_UTIL_H
#define COMPILER_LOYX_UTIL_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
typedef char *c_string;
c_string String(char *);

//typedef char bool;
//#define TRUE 1
//#define FALSE 0

void *checked_malloc(unsigned );

typedef struct U_boolList_ *U_boolList;
struct U_boolList_{bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);
void* U_reverseList(void*);


#endif //COMPILER_LOYX_UTIL_H
