//
// Created by loyx on 2020/3/24.
//

#ifndef COMPILER_LOYX_UTIL_H
#define COMPILER_LOYX_UTIL_H

#include <cassert>
#include <cstdlib>

typedef char *c_string;
c_string String(char *);


void *checked_malloc(unsigned );

typedef struct U_stringList_ *U_stringList ;
struct U_stringList_{c_string head; U_stringList tail;};
U_stringList U_StringList(c_string head, U_stringList tail);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_{bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);
void* U_reverseList(void*);


/**
 * 底下提供了一个简单的int pair及其list
 * 主要是我忘了stl的细节，你们有人改成stl也行
 *
 * 目前的用处是全局数组的frag在用
 * --loyx 2020/7/23
 * */
typedef struct U_intPair_ *U_intPair;
struct U_intPair_{int x, y;};
U_intPair U_IntPair(int x, int y);

typedef struct U_pairList_ *U_pairList;
struct U_pairList_{U_intPair head; U_pairList tail;};
U_pairList U_PairList(U_intPair head, U_pairList tail);


#endif //COMPILER_LOYX_UTIL_H
