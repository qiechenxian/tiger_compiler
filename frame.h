//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_FRAME_H
#define COMPILER_LOYX_FRAME_H

#include "util.h"
#include "temp.h"
#include "tree.h"

/** 架构参数 */



typedef struct F_frame_* F_frame;
typedef struct F_access_* F_access;
typedef  struct F_frag_* F_frag;
typedef  struct F_fragList_* F_fragList;
typedef struct F_accessList_ *F_accessList;
struct F_accessList_{F_access head; F_accessList tail;};

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_getName(F_frame frame);
F_accessList F_getFormals(F_frame frame);
F_access F_allocLocal(F_frame frame, bool escape);
F_access F_allocGlobal();


//Temp_temp /** Frame pointer */
Temp_temp F_FP();
Temp_temp F_RV();
int get_word_size();
T_exp F_Exp(F_access acc, T_exp framePtr);
F_accessList F_formals(F_frame f);

//F_frag片段信息相关
struct F_frag_{
    enum {F_stringFrag,F_procFrag, F_globalFrag}kind;
    union {
        struct{Temp_label label; c_string str;}stringg;
        struct{T_stm body;F_frame frame;}proc;
        struct {Temp_label label; /**todo 缺少初值*/}global;
    }u;
};
struct F_fragList_{F_frag head;F_fragList tail;};
F_fragList F_FragList(F_frag head,F_fragList tail);
F_frag F_StringFrag(Temp_label label,c_string str);
F_frag F_ProcFrag(T_stm body,F_frame frame);
F_frag F_GlobalFrag(Temp_label label);

#endif //COMPILER_LOYX_FRAME_H
