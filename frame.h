//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_FRAME_H
#define COMPILER_LOYX_FRAME_H

#include "util.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"

/** 架构参数 */


//栈的声明

typedef struct F_access_* F_access;
typedef struct F_frame_* F_frame;
typedef struct F_accessList_ *F_accessList;
struct F_accessList_{F_access head; F_accessList tail;};
static F_accessList F_AccessList(F_access head, F_accessList tail);
int F_accessOffset(F_access a);
void F_setMemArgs(F_frame frame);
Temp_temp F_accessReg(F_access a);
bool F_accessIsReg(F_access a);

void F_setFrameCalleeArgs(F_frame frame, int callee_args);
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_getName(F_frame frame);
F_accessList F_getFormals(F_frame frame);
F_access F_allocLocal(F_frame frame, bool escape, int size);
F_access F_allocGlobal(S_symbol global);
Temp_label F_getGlobalLabel(F_access fa);

//F_frag片段信息相关
typedef  struct F_frag_* F_frag;
typedef  struct F_fragList_* F_fragList;
struct F_frag_{
    enum {F_stringFrag,F_procFrag, F_globalFrag}kind;
    union {
        struct{Temp_label label; c_string str;}stringg;
        struct{T_stm body;F_frame frame;}proc;
        struct {Temp_label label; int size; U_pairList init_values;bool comm;}global;
        /// 假设没有int a[1]这种东西 --loyx 2020/7/22
    }u;
};
struct F_fragList_{F_frag head;F_fragList tail;};
F_fragList F_FragList(F_frag head,F_fragList tail);
F_frag F_StringFrag(Temp_label label,c_string str);
F_frag F_ProcFrag(T_stm body,F_frame frame);
F_frag F_GlobalFrag(Temp_label label, int size, U_pairList init_values, bool comm);


//Temp_temp /** Frame pointer */
//机器相关，增加寄存器
extern Temp_map F_tempMap;
Temp_tempList F_registers(void);

Temp_temp F_FP(void);
Temp_temp F_SP(void);
Temp_temp F_ZERO(void);
Temp_temp F_LR(void);
Temp_temp F_RV(void);
Temp_temp F_PC();
Temp_temp F_R0();
Temp_temp F_R1();
Temp_temp F_R2();
Temp_temp F_R3();
Temp_temp F_R(c_string save);
Temp_temp F_R9();
void F_initRegisters(void);
Temp_map F_initialRegisters(F_frame f);
Temp_tempList F_callersaves(void);
Temp_tempList F_calleesaves(void);


int get_word_size();
T_exp F_Exp(F_access acc, T_exp framePtr);
T_exp F_expWithIndex(F_access acc, T_exp framePtr, int index);
F_accessList F_formals(F_frame f);

T_stm F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);
Temp_label get_done_label(F_frame f_frame);
int get_offset(F_access g_access);
T_exp F_fp_address(F_access acc,int index);
int look_for_f_offset(Temp_temp temp,F_frame f);
#endif //COMPILER_LOYX_FRAME_H
