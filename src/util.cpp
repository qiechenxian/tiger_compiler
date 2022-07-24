//
// Created by loyx on 2020/3/24.
//
#include <cstring>
#include "util.h"
#include "errormsg.h"

void* checked_malloc(unsigned len){
    void* p = malloc(len);
    EM_ASSERT_CODE=-98;
    assert(p);
    return p;
}

c_string String(char *str){
    unsigned len = strlen(str)+10;
    c_string p = (c_string)checked_malloc(len * sizeof(char));
    strcpy(p, str);
    return p;
}

U_boolList U_BoolList(bool head, U_boolList tail){
    U_boolList p = (U_boolList)checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

/**
 * reverse list unsafe!!!!
 * */
struct U_tempList{void* head; struct U_tempList* tail;};
void* U_reverseList(void* list){
    struct U_tempList *rev = nullptr, *pre, *iter = (struct U_tempList*)list;
    while (iter){pre = iter; iter = iter->tail; pre->tail = rev; rev = pre;}
    return (void*)rev;
}

U_intPair U_IntPair(int x, int y){
    auto pair = (U_intPair)checked_malloc(sizeof(U_intPair_));
    pair->x = x;
    pair->y = y;
    return pair;
}
U_pairList U_PairList(U_intPair head, U_pairList tail){
    auto list = (U_pairList)checked_malloc(sizeof(U_pairList_));
    list->head = head;
    list->tail = tail;
    return list;
}
