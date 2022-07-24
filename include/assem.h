//
// Created by zcjsh on 2020/7/21.
//

#ifndef UNTITLED_ASSEM_H
#define UNTITLED_ASSEM_H

#include <stdio.h>
#include <string.h>
#include "errormsg.h"
#include "temp.h"
#include "table.h"

typedef struct AS_targets_ *AS_targets;
struct AS_targets_ {
    Temp_labelList labels;
} ;

AS_targets AS_Targets(Temp_labelList labels);


/**
 * 设计的一种数据类型AS_instr，用于表示”没有指定寄存器的汇编语言指令“
 * OPER包含汇编语言指令assem、操作数寄存器表src、结果寄存器表dst，jump是可以转移到的目标的标号表
 * LABEL是程序中转移可以到达的位置
 * MOVE和OPER类似但只进行数据传送，如果dst和src相同，则此move指令可以删去
 */
typedef struct AS_instr_ *AS_instr;
struct AS_instr_ {
    enum {
        I_OPER, I_LABEL, I_MOVE
    } kind;
    enum {
        O_mov,  O_add,O_sub,O_div,O_str,O_ldr,O_mod,O_others,O_lsl
    } kindl;
    enum {
        OP_NG = -1, OP_STR, OP_LTR
    };
    union {
        struct {
            c_string assem;
            Temp_tempList dst, src;
            AS_targets jumps;
            bool info_tag;
            Temp_tempList div_dst,div_src;
        } OPER;
        struct {
            c_string assem;
            Temp_label label;
        } LABEL;
        struct {
            c_string assem;
            Temp_tempList dst, src;
        } MOVE;
    } u;
    int op_kind;
    bool isDead;
};

AS_instr AS_Oper(c_string a, Temp_tempList d, Temp_tempList s, AS_targets j, bool tag = false,int op_kind = AS_instr_::OP_NG);

AS_instr AS_Label(c_string a, Temp_label label);

AS_instr AS_Move(c_string a, Temp_tempList d, Temp_tempList s, bool tag = false);

// 调用AS_print可以将一条汇编指令表示为字符串的形式并输出
// 需要在temp.h中描述对临时变量映射操作的函数
void AS_string(AS_instr i, Temp_map m, char * result);

typedef struct AS_instrList_ *AS_instrList;
struct AS_instrList_ {
    AS_instr head;
    AS_instrList tail;
};

AS_instrList AS_InstrList(AS_instr head, AS_instrList tail);

AS_instrList AS_instrUnion(AS_instrList ta, AS_instrList tb);
AS_instrList AS_instrMinus(AS_instrList ta, AS_instrList tb);
AS_instrList AS_instrIntersect(AS_instrList ta, AS_instrList tb);
bool AS_instrInList(AS_instr i, AS_instrList il);

//把list b 拼接到 list a后边
AS_instrList AS_splice(AS_instrList a, AS_instrList b);

//
void AS_printInstrList(FILE *out, AS_instrList iList, Temp_map m);

typedef struct AS_proc_ *AS_proc;
struct AS_proc_ {
    c_string prolog;
    AS_instrList body;
    c_string epilog;
};

AS_proc AS_Proc(c_string p, AS_instrList b, c_string e);

#endif //UNTITLED_ASSEM_H
