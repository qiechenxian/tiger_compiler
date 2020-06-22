//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_TEMP_H
#define COMPILER_LOYX_TEMP_H

#include "symbol.h"


typedef struct Temp_temp_* Temp_temp;
Temp_temp Temp_newTemp();

typedef  S_symbol Temp_label;
Temp_label Temp_newLabel();
Temp_label Temp_namedLabel(c_string name);
c_string Temp_labelString(Temp_label s);

typedef struct Temp_tempList_ *Temp_tempList;
struct Temp_tempList_{Temp_temp head; Temp_tempList tail;};
Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail);

typedef struct Temp_labelList_ *Temp_labelList;
struct Temp_labelList_{Temp_label head; Temp_labelList tail;};
Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail);

#endif //COMPILER_LOYX_TEMP_H
