//
// Created by loyx on 2020/4/25.
//

#ifndef COMPILER_LOYX_TABLE_H
#define COMPILER_LOYX_TABLE_H

typedef struct TAB_table_ *TAB_table;
TAB_table TAB_empty();
void TAB_enter(TAB_table t, void *key, void *value);
void *TAB_look(TAB_table t, void *key);
void *TAB_pop(TAB_table t);


#endif //COMPILER_LOYX_TABLE_H
