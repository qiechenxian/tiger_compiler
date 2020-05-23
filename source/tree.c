//
// Created by loyx on 2020/5/8.
//

#include "../include/tree.h"
T_expList T_ExpList(T_exp head, T_expList tail)
{
    T_expList p = (T_expList)checked_malloc(sizeof *p);
    p->head = head;
    p->tail = tail;
    return p;
}

T_stmList T_StmList(T_stm head, T_stmList tail)
{
    T_stmList p = (T_stmList)checked_malloc(sizeof *p);
    p->head = head;
    p->tail = tail;
    return p;
}

T_stm T_Seq(T_stm left, T_stm right)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_SEQ;
    p->u.SEQ.left = left;
    p->u.SEQ.right = right;
    return p;
}

T_stm T_Label(Temp_label label)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_LABEL;
    p->u.LABEL = label;
    return p;
}

T_stm T_Jump(T_exp exp, Temp_labelList labels)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_JUMP;
    p->u.JUMP.exp = exp;
    p->u.JUMP.jumps = labels;
    return p;
}

T_stm T_Cjump(T_relOp op, T_exp left, T_exp right, Temp_label trueLabel, Temp_label falseLabel)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_CJUMP;
    p->u.CJUMP.op = op;
    p->u.CJUMP.left = left;
    p->u.CJUMP.right = right;
    p->u.CJUMP.trueLabel = trueLabel;
    p->u.CJUMP.falseLabel = falseLabel;
    return p;
}

T_stm T_Move(T_exp dst, T_exp src)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_MOVE;
    p->u.MOVE.dst = dst;
    p->u.MOVE.src = src;
    return p;
}

T_stm T_Exp(T_exp exp)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_EXP;
    p->u.EXP = exp;
    return p;
}

T_exp T_Binop(T_binOp op, T_exp left, T_exp right)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_BINOP;
    p->u.BINOP.op = op;
    p->u.BINOP.left = left;
    p->u.BINOP.right = right;
    return p;
}

T_exp T_Mem(T_exp exp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_MEM;
    p->u.MEM = exp;
    return p;
}

T_exp T_Temp(Temp_temp temp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_TEMP;
    p->u.TEMP = temp;
    return p;
}

T_exp T_Eseq(T_stm stm, T_exp exp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_ESEQ;
    p->u.ESEQ.stm = stm;
    p->u.ESEQ.exp = exp;
    return p;
}

T_exp T_Name(Temp_label name)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_NAME;
    p->u.NAME = name;
    return p;
}

T_exp T_Const(int consti)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_CONST;
    p->u.CONST = consti;
    return p;
}

T_exp T_Call(T_exp fun, T_expList args)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_CALL;
    p->u.CALL.fun = fun;
    p->u.CALL.args = args;
    return p;
}
