//
// Created by loyx on 2020/5/8.
//

#include "tree.h"
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
    p->kind = T_stm_::T_SEQ;
    p->u.SEQ.left = left;
    p->u.SEQ.right = right;
    return p;
}

T_stm T_Label(Temp_label label)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_LABEL;
    p->u.LABEL = label;
    return p;
}

T_stm T_Jump(T_exp exp, Temp_labelList labels)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_JUMP;
    p->u.JUMP.exp = exp;
    p->u.JUMP.jumps = labels;
    return p;
}

T_stm T_Cjump(T_relOp op, T_exp left, T_exp right, Temp_label trueLabel, Temp_label falseLabel)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_CJUMP;
    p->u.CJUMP.op = op;
    p->u.CJUMP.left = left;
    p->u.CJUMP.right = right;
    p->u.CJUMP.trues = trueLabel;
    p->u.CJUMP.falses = falseLabel;
    return p;
}
T_stm T_Stmfd(T_expList exp)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind=T_stm_::T_STMFD;
    p->u.STMFD.exp_list=exp;
    return p;
}
T_stm T_Ldmfd(T_expList exp)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_LDMFD;
    p->u.LDMFD.exp_list=exp;
    return p;
}
T_stm T_Move(T_exp dst, T_exp src)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_MOVE;
    p->u.MOVE.dst = dst;
    p->u.MOVE.src = src;
    return p;
}

T_stm T_Exp(T_exp exp)
{
    T_stm p = (T_stm)checked_malloc(sizeof *p);
    p->kind = T_stm_::T_EXP;
    p->u.EXP = exp;
    return p;
}

T_exp T_Binop(T_binOp op, T_exp left, T_exp right)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_BINOP;
    p->u.BINOP.op = op;
    p->u.BINOP.left = left;
    p->u.BINOP.right = right;
    return p;
}


T_exp T_Mem(T_exp exp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_MEM;
    p->u.MEM = exp;
    return p;
}

T_exp T_Temp(Temp_temp temp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_TEMP;
    p->u.TEMP = temp;
    return p;
}

T_exp T_Eseq(T_stm stm, T_exp exp)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_ESEQ;
    p->u.ESEQ.stm = stm;
    p->u.ESEQ.exp = exp;
    return p;
}

T_exp T_Name(Temp_label name)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_NAME;
    p->u.NAME = name;
    return p;
}

T_exp T_Const(int consti)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_CONST;
    p->u.CONST = consti;
    return p;
}

T_exp T_Call(T_exp fun, T_expList args)
{
    T_exp p = (T_exp)checked_malloc(sizeof *p);
    p->kind = T_exp_::T_CALL;
    p->u.CALL.fun = fun;
    p->u.CALL.args = args;
    return p;
}
T_relOp T_not_op(T_relOp op)//T_lt, T_le, T_gt, T_ge, T_eq, T_ne,T_not,T_and, T_or,
{
    switch (op) {
        case T_lt:return T_ge;
        case T_le:return T_gt;
        case T_ge:return T_lt;
        case T_gt:return T_le;
        case T_eq:return T_ne;
        case T_ne:return T_eq;
        default: //永远不应该被执行
            assert(0);
            return T_eq;
    }
}
