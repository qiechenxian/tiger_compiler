//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_TEMP_H
#define COMPILER_LOYX_TEMP_H

#include "symbol.h"
#include "stdio.h"

typedef struct Temp_temp_* Temp_temp;
Temp_temp Temp_newTemp();

typedef struct Temp_tempList_ *Temp_tempList;
struct Temp_tempList_{Temp_temp head; Temp_tempList tail;};
Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail);
Temp_tempList Temp_reverseList(Temp_tempList t);

bool Temp_equal(Temp_tempList ta, Temp_tempList tb);
Temp_tempList Temp_union(Temp_tempList ta, Temp_tempList tb);
Temp_tempList Temp_intersect(Temp_tempList ta, Temp_tempList tb);
Temp_tempList Temp_minus(Temp_tempList ta, Temp_tempList tb);
bool Temp_inList(Temp_temp t, Temp_tempList tl);

typedef  S_symbol Temp_label;
Temp_label Temp_newLabel();
Temp_label Temp_newStringLabel();
Temp_label Temp_namedLabel(c_string name);
c_string Temp_labelString(Temp_label s);

typedef struct Temp_labelList_ *Temp_labelList;
struct Temp_labelList_{Temp_label head; Temp_labelList tail;};
Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail);

//用于assem中寄存器临时变量映射
typedef struct Temp_map_ *Temp_map;
Temp_map Temp_empty(void);
Temp_map Temp_layerMap(Temp_map over, Temp_map under);
void Temp_enter(Temp_map m, Temp_temp t, c_string s);
c_string Temp_look(Temp_map m, Temp_temp t);
void Temp_dumpMap(FILE *out, Temp_map m);

void Temp_enterPtr(Temp_map m, Temp_temp t, void *ptr);
void* Temp_lookPtr(Temp_map m, Temp_temp t);

Temp_map Temp_name(void);
Temp_temp Temp_new_special(c_string s, int num);
#endif //COMPILER_LOYX_TEMP_H
