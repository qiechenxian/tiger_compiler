//
// Created by loyx on 2020/4/25.
//

#include <string.h>
#include "../include/symbol.h"
#include "../include/table.h"

struct S_symbol_{string name; S_symbol next;};
static S_symbol newSymbol(string name, S_symbol next){
    S_symbol s = checked_malloc(sizeof(*s));
    s->name = name; s->next = next;
    return s;
}

#define SIZE 125
static S_symbol hashtable[SIZE];
static unsigned hash(char* s0){
    unsigned h = 0;
    for(char* s = s0; *s; s++){
        h = h*65599 + *s;
    }
    return h;
}

S_symbol S_Symbol(string name){
    unsigned index = hash(name) % SIZE;
    S_symbol symbols = hashtable[index];
    for(S_symbol sym = symbols; sym; sym = sym->next){
        if(0 == strcmp(sym->name, name)){
            return sym;
        }
    }
    S_symbol new_sym = newSymbol(name, symbols);
    hashtable[index] = new_sym;
    return new_sym;
}
string S_getName(S_symbol sym){
    return sym->name;
}
S_table S_empty(){
    return (S_table) TAB_empty();
}
void S_enter(S_table t, S_symbol sym, void* value){
    TAB_enter((TAB_table) t, sym, value);
}
void* S_look(S_table t, S_symbol sym){
    return TAB_look((TAB_table) t, sym);
}

static struct S_symbol_ mark_sym = {"<mark>", 0};
void S_beginScope(S_table t){
    S_enter(t, &mark_sym, NULL);
}
void S_endScope(S_table t){
    S_symbol s;
    do {
        s = TAB_pop((TAB_table) t);
    }while (s != &mark_sym);
}
