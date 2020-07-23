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
F_access F_allocGlobal(S_symbol global);
Temp_label F_getGlobalLabel(F_access fa);


//Temp_temp /** Frame pointer */
Temp_temp F_FP(void);
Temp_temp F_SP(void);
Temp_temp F_ZERO(void);
Temp_temp F_RA(void);
Temp_temp F_RV(void);
int get_word_size();
T_exp F_Exp(F_access acc, T_exp framePtr);
F_accessList F_formals(F_frame f);

//F_frag片段信息相关
struct F_frag_{
    enum {F_stringFrag,F_procFrag, F_globalFrag}kind;
    union {
        struct{Temp_label label; c_string str;}stringg;
        struct{T_stm body;F_frame frame;}proc;
        struct {Temp_label label; int size; U_pairList init_values;}global;
        /// 假设没有int a[1]这种东西 --loyx 2020/7/22
    }u;
};
struct F_fragList_{F_frag head;F_fragList tail;};
F_fragList F_FragList(F_frag head,F_fragList tail);
F_frag F_StringFrag(Temp_label label,c_string str);
F_frag F_ProcFrag(T_stm body,F_frame frame);
F_frag F_GlobalFrag(Temp_label label, int size, U_pairList init_values);


T_stm F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);
#endif //COMPILER_LOYX_FRAME_H
