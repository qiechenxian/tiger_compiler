//
// Created by loyx on 2020/4/25.
//

#include <stdio.h>
#include "util.h"
#include "table.h"

#define TAB_SIZE 127

typedef struct binder_* binder;
struct binder_{
    void* key;
    void* value;
    binder next;
    void* prev_top;
};
static binder Binder(void* key, void* value, binder next, void *prev_top){
    binder b = checked_malloc(sizeof(*b));
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
    TAB_table t = checked_malloc(sizeof(*t));
    t->top = NULL;
    for(int i = 0; i < TAB_SIZE; i++)
        t->table[i] = NULL;
    return t;
}
void TAB_enter(TAB_table t, void* key, void* value){
    assert(t && key);
    unsigned index = ((unsigned)key) % TAB_SIZE;
    t->table[index] = Binder(key, value, t->table[index], t->top);
    t->top = key;
}
void* TAB_look(TAB_table t, void* key){
    assert(t && key);
    unsigned index = ((unsigned)key) % TAB_SIZE;
    for(binder b = t->table[index]; b; b = b->next){
        if(b->key == key) return b->value;
    }
    return NULL;
}
void* TAB_pop(TAB_table t){
    assert(t);
    void* pop_key = t->top;
    assert(pop_key);
    unsigned index = ((unsigned)pop_key) % TAB_SIZE;
    binder pop_binder = t->table[index];
    assert(pop_binder);
    t->table[index] = pop_binder->next;
    t->top = pop_binder->prev_top;
    //todo free?
    return pop_binder->key;
}