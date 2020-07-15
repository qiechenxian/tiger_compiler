//
// Created by Administrator on 14/7/2020.
//

#ifndef COMPILER_BUG_CONSTEXP_ASSEM_H
#define COMPILER_BUG_CONSTEXP_ASSEM_H
#include <stdio.h>
#include "temp.h"

typedef struct {Temp_labelList labels;} *AS_targets;
AS_targets AS_Targets(Temp_labelList labels);

typedef struct AS_instr_ *AS_instr;

struct AS_instr_ { enum {I_OPER, I_LABEL, I_MOVE} kind;
    union {struct {c_string  assem; Temp_tempList dst, src;
            AS_targets jumps;} OPER;
        struct {c_string assem; Temp_label label;} LABEL;
        struct {c_string assem; Temp_tempList dst, src;} MOVE;
    } u;
};

AS_instr AS_Oper(c_string a, Temp_tempList d, Temp_tempList s, AS_targets j);
AS_instr AS_Label(c_string a, Temp_label label);
AS_instr AS_Move(c_string a, Temp_tempList d, Temp_tempList s);

void AS_print(FILE *out, AS_instr i, Temp_map m);

typedef struct AS_instrList_ *AS_instrList;
struct AS_instrList_ { AS_instr head; AS_instrList tail;};
AS_instrList AS_InstrList(AS_instr head, AS_instrList tail);

AS_instrList AS_splice(AS_instrList a, AS_instrList b);
void AS_printInstrList (FILE *out, AS_instrList iList, Temp_map m);

typedef struct AS_proc_ *AS_proc;
struct AS_proc_ {
    c_string prolog;
    AS_instrList body;
    c_string epilog;
};

AS_proc AS_Proc(c_string p, AS_instrList b, c_string e);
#endif //COMPILER_BUG_CONSTEXP_ASSEM_H
