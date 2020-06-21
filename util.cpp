//
// Created by loyx on 2020/3/24.
//
#include <string.h>
#include "util.h"

void* checked_malloc(unsigned len){
    void* p = malloc(len);
    assert(p);
    return p;
}

string String(char *str){
    unsigned len = strlen(str);
    string p = (string)checked_malloc(len*sizeof(char));
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
    struct U_tempList *rev = NULL, *pre, *iter = (struct U_tempList*)list;
    while (iter){pre = iter; iter = iter->tail; pre->tail = rev; rev = pre;}
    return (void*)rev;
}
