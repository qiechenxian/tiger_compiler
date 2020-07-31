//
// Created by loyx on 2020/4/25.
//

#include <stdio.h>
#include "util.h"
#include "table.h"

#define TAB_SIZE 16

typedef struct binder_* binder;
struct binder_{
    void* key;
    void* value;
    binder next;
    void* prev_top;
};
static binder Binder(void* key, void* value, binder next, void *prev_top){
    binder b = (binder)checked_malloc(sizeof(*b));
    b->key = key;
    b->value = value;
    b->next = next;
    b->prev_top = prev_top;
    return b;
}


struct TAB_table_{
    binder table[TAB_SIZE];
    void* top;
};
TAB_table TAB_empty(){
    TAB_table t = (TAB_table)checked_malloc(sizeof(*t));
    t->top = NULL;
    for(int i = 0; i < TAB_SIZE; i++)
        t->table[i] = NULL;
    return t;
}
void TAB_enter(TAB_table t, void* key, void* value){
    assert(t && key);
    long long index = ((long long)key) % TAB_SIZE;
    t->table[index] = Binder(key, value, t->table[index], t->top);
    t->top = key;
}
void* TAB_look(TAB_table t, void* key){
    assert(t && key);
    long long index = ((long long)key) % TAB_SIZE;
    for(binder b = t->table[index]; b; b = b->next){
        if(b->key == key) return b->value;
    }
    return NULL;
}
void* TAB_pop(TAB_table t){
    assert(t);
    void* pop_key = t->top;
    assert(pop_key);
    long long index = ((long long)pop_key) % TAB_SIZE;
    binder pop_binder = t->table[index];
    assert(pop_binder);
    t->table[index] = pop_binder->next;
    t->top = pop_binder->prev_top;
    //todo free?
    return pop_binder->key;
}

void TAB_dump(TAB_table t, void (*show)(void *key, void *value))
{
    void *k=t->top;
    int index=((long long) k)%TAB_SIZE;
    binder b=t->table[index];
    if(b==NULL) return;
    t->table[index]=b->next;
    t->top=b->prev_top;
    show(b->key,b->value);
    TAB_dump(t,show);
    assert(t->top==b->prev_top&&t->table[index]==b->next);
    t->top=k;
    t->table[index]=b;
}