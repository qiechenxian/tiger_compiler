//
// Created by zcjsh on 2020/7/21.
//
#include "codegen.h"

// 添加instr到指令表中
static AS_instrList iList = NULL, last = NULL;

static void emit(AS_instr instr) {
    if (last != NULL) {
        last->tail = AS_InstrList(instr, NULL);
        last = last->tail;
    } else {
        iList = AS_InstrList(instr, NULL);
        last = iList;
    }
}

Temp_tempList L(Temp_temp h, Temp_tempList t) {
    return Temp_TempList(h, t);
}


// 用于表达式的匹配，每种情形的从句将匹配一个瓦片
static Temp_temp munchExp(T_exp e);

// 用于语句的匹配
static void munchStm(T_stm s);

// 用于过程调用中参数传递到正确位置
static Temp_tempList munchArgs(int i, T_expList args);

//调用库函数
static void call_lib(c_string fun, Temp_temp rsreg, Temp_temp reg1, Temp_temp reg2);

/*
 * 快速匹配没做
 */
AS_instrList F_codegen(F_frame f, T_stmList stmList) {
    AS_instrList list;
    T_stmList stmList1;
    for (stmList1 = stmList; stmList1; stmList1 = stmList1->tail) {
        munchStm(stmList1->head);
    }
    list = iList;
    last = NULL;
    iList = last;
    return list;
}

/*
 * 使用的指令包括
 * MOV ADD SUB CMP AND ORR B BL LDR STR
 */
static Temp_temp munchExp(T_exp e) {
    char *inst = (char *) checked_malloc(sizeof(char) * 120);
    char *inst2 = (char *) checked_malloc(sizeof(char) * 120);
    char *inst3 = (char *) checked_malloc(sizeof(char) * 120);
    switch (e->kind) {
        case T_exp_::T_MEM: {
            T_exp mem = e->u.MEM;
            if (mem->kind == T_exp_::T_BINOP) {
                if (mem->u.BINOP.op == T_add && mem->u.BINOP.right->kind == T_exp_::T_CONST) {
                    /* MEM(BINOP(PLUS,e1,CONST(i))) */
                    T_exp e1 = mem->u.BINOP.left;
                    int i = mem->u.BINOP.right->u.CONST;
                    Temp_temp r = Temp_newTemp();
                    sprintf(inst, "LDR 'd0,['s0,#%d]\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                    return r;
                } else if (mem->u.BINOP.op == T_add && mem->u.BINOP.left->kind == T_exp_::T_CONST) {
                    /* MEM(BINOP(PLUS,CONST(i),e1)) */
                    T_exp e1 = mem->u.BINOP.right;
                    int i = mem->u.BINOP.left->u.CONST;
                    Temp_temp r = Temp_newTemp();
                    sprintf(inst, "LDR 'd0,['s0,#%d]\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                    return r;
                } else {
                    /* MEM(e1) */
                    T_exp e1 = mem;
                    Temp_temp r = Temp_newTemp();
                    sprintf(inst, "LDR 'd0,['s0]\n");
                    emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                    return r;
                }
            } else if (mem->kind == T_exp_::T_CONST) {
                /* MEM(CONST(i)) */
                int i = mem->u.CONST;
                Temp_temp r = Temp_newTemp();
                //无0寄存器怎么表示0+const?
                sprintf(inst, "LDR 'd0,[#%d]\n", i);
                emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                return r;
            }
            else {
                /* MEM(e1) */
                T_exp e1 = mem;
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "LDR 'd0,['s0]\n");
                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            }
        }
        case T_exp_::T_BINOP: {
            if (e->u.BINOP.op == T_add && e->u.BINOP.right->kind == T_exp_::T_CONST) {
                /* BINOP(PLUS,e1,CONST(i)) */
                T_exp e1 = e->u.BINOP.left;
                int i = e->u.BINOP.right->u.CONST;
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "ADD 'd0,'s0,#%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            } else if (e->u.BINOP.op == T_add && e->u.BINOP.left->kind == T_exp_::T_CONST) {
                /* BINOP(PLUS,CONST(i),e1) */
                T_exp e1 = e->u.BINOP.right;
                int i = e->u.BINOP.left->u.CONST;
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "ADD 'd0,'s0,#%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            } else if (e->u.BINOP.op == T_sub && e->u.BINOP.right->kind == T_exp_::T_CONST) {
                /* BINOP(MINUS,e1,CONST(i)) */
                T_exp e1 = e->u.BINOP.left;
                int i = e->u.BINOP.right->u.CONST;
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "SUB 'd0,'s0,#%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            } else if (e->u.BINOP.op == T_sub && e->u.BINOP.left->kind == T_exp_::T_CONST) {
                T_exp e1 = e->u.BINOP.right;
                int i = e->u.BINOP.left->u.CONST;
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "SUB 'd0,'s0,#%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            } else if (e->u.BINOP.op == T_add) {
                /* BINOP(PLUS,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);
                sprintf(inst, "ADD 'd0,'s0,'s1\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else if (e->u.BINOP.op == T_sub) {
                /* BINOP(MINUS,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);
                sprintf(inst, "SUB 'd0,'s0,'s1\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else if (e->u.BINOP.op == T_mul) {
                /* BINOP(MUL,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);
                sprintf(inst, "MUL 'd0,'s0,'s1\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else if (e->u.BINOP.op == T_div) {
                /* BINOP(DIV,e1,e2) */
                //嵌入汇编divsi3
                //TODO 需测试div能否正常传递参数
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);
                call_lib("__divsi3", r, r1, r2);
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else {
                EM_error(0, "codegen not munch exp when kind is T_BINOP.");
                break;
            }
        }
        case T_exp_::T_CONST: {
            /* CONST(i) */
            int i = e->u.CONST;
            Temp_temp r = Temp_newTemp();
            sprintf(inst, "MOV 'd0,#%d\n", i);
            emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
            return r;
        }
        case T_exp_::T_TEMP: {
            /* TEMP(t) */
            return e->u.TEMP;
        }
        case T_exp_::T_NAME: {
            /**
             * 此节点表示NAME(lab)， 意义为全局变量lab的地址
             * 对应的汇编指令为： ldr r1, =lab
             * arm汇编中全局变量的lab类似一个宏或者立即数，内容为该全局变量的地址。
             * 但经过试验发现，全局变量的label(地址)不用使用 ldr r1, lab指令，直接将lab地址的内容加载到r1中
             * 只能通过两步：
             *      1. ldr r1, =lab  //先将全局变量的lab(地址)放到寄存器中
             *      2. ldr r2, [r1]  //再根据全局变量的地址，取其中的内容
             *
             * 此节点的意义是先将全局变量的地址放在一个寄存器中，供之后使用
             * --loyx 2020/7/26
             */
            /* NAME(lab) */
            Temp_label label = e->u.NAME;
            Temp_temp r = Temp_newTemp();
            //label处理？
            sprintf(inst, "LDR 'd0,=%s\n", Temp_labelString(label));
            emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
            return r;
        }
        case T_exp_::T_CALL:
            //call 在stm中覆盖
        case T_exp_::T_ESEQ:
            //无
        default: {
            EM_error(0, "not find munchExp kind.");
            assert(0);
        }
    }
}

/*
 * 使用的指令包括
 */
static void munchStm(T_stm s) {
    char *inst = (char *) checked_malloc(sizeof(char) * 120);
    char *inst2 = (char *) checked_malloc(sizeof(char) * 120);
    char *inst3 = (char *) checked_malloc(sizeof(char) * 120);
    switch (s->kind) {
        case T_stm_::T_MOVE: {
            T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
            if (dst->kind == T_exp_::T_MEM) {
                if (dst->u.MEM->kind == T_exp_::T_BINOP
                    && dst->u.MEM->u.BINOP.op == T_add
                    && dst->u.MEM->u.BINOP.right->kind == T_exp_::T_CONST) {
                    /* MOVE(MEM(BINOP(PLUS,e1,CONST(i))),e2) */
                    T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
                    int i = dst->u.MEM->u.BINOP.right->u.CONST;
                    sprintf(inst, "STR 's0,['d0,#%d]\n", i);
                    emit(AS_Oper(inst, L(munchExp(e1), NULL), L(munchExp(e2), NULL), NULL));
                } else if (dst->u.MEM->kind == T_exp_::T_BINOP
                           && dst->u.MEM->u.BINOP.op == T_add
                           && dst->u.MEM->u.BINOP.left->kind == T_exp_::T_CONST) {
                    /* MOVE(MEM(BINOP(PLUS,CONST(i),e1)),e2) */
                    T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
                    int i = dst->u.MEM->u.BINOP.left->u.CONST;
                    sprintf(inst, "STR 's0,['d0,#%d]\n", i);
                    emit(AS_Oper(inst, L(munchExp(e1), NULL), L(munchExp(e2), NULL), NULL));
                } else if (src->kind == T_exp_::T_MEM) {
                    /* MOVE(MEM(e1), MEM(e2)) */
                    T_exp e1 = dst->u.MEM, e2 = src->u.MEM;
                    Temp_temp r = Temp_newTemp();
                    Temp_temp r1 = munchExp(e1);
                    Temp_temp r2 = munchExp(e2);
                    sprintf(inst, "LDR 'd0,['s0]\n");
                    emit(AS_Move(inst, L(r, NULL), L(r2, NULL)));
                    sprintf(inst2, "STR ‘d0,['s0]\n");
                    emit(AS_Oper(inst2, L(r, NULL), L(r1, NULL), NULL));
                } else if (dst->u.MEM->kind == T_exp_::T_CONST) {
                    /* MOVE(MEM(CONST(i)), e1) */
                    T_exp e1 = src;
                    int i = dst->u.MEM->u.CONST;\
                    //TODO 无0寄存器怎么表示0+const?
                    sprintf(inst, "STR 's0,[#%d]\n", i);
                    emit(AS_Oper(inst, NULL, L(munchExp(e1), NULL), NULL));
                }

                /**
                 * 此处交给MOVE(MEM(e1), e2)处理
                 */
//                else if(dst->u.MEM->kind==T_exp_::T_NAME)
//                {
//                    /* MOVE(MEM(NAME(lab)), e1) */
//                    T_exp e1=src;
//                    Temp_temp r1=munchExp(e1);
//                    Temp_label label = dst->u.MEM->u.NAME;
//                    sprintf(inst,"LDR 'd0,=%s\n",Temp_labelString(label));
//                    emit(AS_Oper(inst,L(r1,NULL),L(r1,NULL),NULL));
//                }
                else {
                    /* MOVE(MEM(e1), e2) */
                    T_exp e1 = dst->u.MEM, e2 = src;
                    Temp_temp r1=munchExp(e1);
                    Temp_temp r2=munchExp(e2);
                    sprintf(inst, "STR 's0,['d0]\n");
                    emit(AS_Oper(inst, L(r1, NULL), L(r2, NULL), NULL));
                }
            } else if (dst->kind == T_exp_::T_TEMP) {
                if (src->kind == T_exp_::T_CALL) {
                    if (src->u.CALL.fun->kind == T_exp_::T_NAME) {
                        /* MOVE(TEMP(t),CALL(NAME(lab),args)) */
                        Temp_label lab = src->u.CALL.fun->u.NAME;
                        T_expList args = src->u.CALL.args;
                        Temp_temp t = dst->u.TEMP;
                        Temp_tempList l = munchArgs(0, args);
                        Temp_tempList calldefs = NULL; // TODO
                        //TODO 函数调用测试能否正确传参
                        sprintf(inst, "BL %s\n", Temp_labelString(lab));
                        emit(AS_Oper(inst, L(F_RA(), calldefs), l, AS_Targets(Temp_LabelList(lab, NULL))));
                        sprintf(inst2, "MOV `d0, `s0\n");
                        emit(AS_Move(inst2, L(t, NULL), L(F_RA(), NULL)));
                    } else {
                        /* MOVE(TEMP(t),CALL(e1,args)) */
                        T_exp e1 = src->u.CALL.fun;
                        T_expList args = src->u.CALL.args;
                        Temp_temp t = dst->u.TEMP;
                        Temp_temp r1 = munchExp(e1);
                        Temp_tempList l = munchArgs(0, args);
                        Temp_tempList calldefs = NULL; // TODO
                        //TODO，此处未写，call(e1,args)节点含义不明
                        EM_error(0,"call(e1,args)节点含义不明");
                        assert(0);
                    }
                } else {
                    /* MOVE(TEMP(i),e2) */
                    T_exp e1 = src;
                    Temp_temp temp = dst->u.TEMP;
                    sprintf(inst, "MOV 'd0, 's0\n");
                    emit(AS_Move(inst, L(temp, NULL), L(munchExp(e1), NULL)));
                }
            } else if (dst->kind == T_exp_::T_NAME) {
                //存全局变量
                /* MOVE(NAME(lab),e1) */
                assert(0);
                /**
                 * IR表示中无MOVE(NAME(lab), e1)这种情况。
                 * NAME(lab)表示全局变量的地址，当存全局变量时，表示为MOVE(MEM(NAME(lab)), e1)
                 * 这样与存局部变量形成统一，同时MEM(NAME(lab))可表示取全局变量
                 * --loyx 2020/7/26
                 */
                T_exp e1 = src;
                Temp_label lab = dst->u.NAME;
                Temp_temp temp = Temp_newTemp();
                sprintf(inst, "LDR 'd0, =%s\n", Temp_labelString(lab));
                emit(AS_Oper(inst, L(temp, NULL), NULL, NULL));
                sprintf(inst2, "STR 's0, ['d0]\n", Temp_labelString(lab));
                emit(AS_Oper(inst2, L(temp, NULL), L(munchExp(e1), NULL), NULL));
            } else {
                assert(0);
            }
            break;
        }
        case T_stm_::T_LABEL: {
            /* LABEL(lab) */
            Temp_label label = s->u.LABEL;
            sprintf(inst, "%s:\n", Temp_labelString(label));
            emit(AS_Label(inst, label));
            break;
        }
        case T_stm_::T_EXP: {
            if (s->u.EXP->kind == T_exp_::T_CALL) {
                T_exp call = s->u.EXP;
                if (call->u.CALL.fun->kind == T_exp_::T_NAME) {
                    /* EXP(CALL(eNAME(lab),args)) */
                    Temp_label lab = call->u.CALL.fun->u.NAME;
                    T_expList args = call->u.CALL.args;
                    Temp_tempList l = munchArgs(0, args);
                    Temp_tempList calldefs = NULL; // TODO
                    // 函数调用？
                    sprintf(inst, "BL %s\n", Temp_labelString(lab));
                    emit(AS_Oper(inst, calldefs, l, AS_Targets(Temp_LabelList(lab, NULL))));
                } else {
                    /* EXP(CALL(e,args)) */
                    T_exp e1 = call->u.CALL.fun;
                    T_expList args = call->u.CALL.args;
                    Temp_temp r1 = munchExp(e1);
                    Temp_tempList l = munchArgs(0, args);
                    Temp_tempList calldefs = NULL; // TODO
                    // TODO 此处未写，call(e1,args)语义不明
                }
            } else {
                /* EXP(e) */
                munchExp(s->u.EXP);
            }
            break;
        }
        case T_stm_::T_JUMP: {
            if (s->u.JUMP.exp->kind == T_exp_::T_NAME) {
                /* JUMP(NAME(lab)) */
                Temp_label lab = s->u.JUMP.exp->u.NAME;
                Temp_labelList jumps = s->u.JUMP.jumps;
                sprintf(inst, "B %s\n", Temp_labelString(lab));
                emit(AS_Oper(inst, NULL, NULL, AS_Targets(jumps)));
            } else {
                /* JUMP(e) */
                T_exp e1 = s->u.JUMP.exp;
                Temp_labelList jumps = s->u.JUMP.jumps;
                // TODO 此处未写，jump(e)语义不明
            }
            break;
        }
        case T_stm_::T_CJUMP: {
            /* CJUMP(op,e1,e2,jt,jf) */
            T_relOp op = s->u.CJUMP.op;
            T_exp e1 = s->u.CJUMP.left;
            T_exp e2 = s->u.CJUMP.right;
            Temp_temp r1 = munchExp(e1);
            Temp_temp r2 = munchExp(e2);
            Temp_label jt = s->u.CJUMP.trues;
            Temp_label jf = s->u.CJUMP.falses;
            sprintf(inst, "CMP 's0,'s1\n");
            emit(AS_Oper(inst, NULL, L(r1, L(r2, NULL)), NULL));

            char *opcode;
            switch (op) {
                case T_eq:
                    opcode = (char*)"BEQ";
                    break;
                case T_ne:
                    opcode = (char*)"BNE";
                    break;
                case T_lt:
                    opcode = (char*)"BLT";
                    break;
                case T_gt:
                    opcode = (char*)"BGT";
                    break;
                case T_le:
                    opcode = (char*)"BLE";
                    break;
                case T_ge:
                    opcode = (char*)"BGE";
                    break;
                default: {
                    EM_error(0, "not find op.");
                    assert(0);
                }
            }
            sprintf(inst2, "%s %s\n", opcode, Temp_labelString(jt));
            emit(AS_Oper(inst2, NULL, NULL, AS_Targets(Temp_LabelList(jt, NULL))));

            sprintf(inst3, "B %s\n", Temp_labelString(jf));
            emit(AS_Oper(inst3, NULL, NULL, AS_Targets(Temp_LabelList(jf, NULL))));
            break;
        }
        default: {
            assert(0);
        }
    }
}

//// 用于过程调用中参数传递到正确位置，寄存器分配，未完成
static Temp_tempList munchArgs(int i, T_expList args) {
    return NULL;
}

/*
 * 调用库函数 resReg=fun(arg1,arg2);
 */
static void call_lib(c_string fun, Temp_temp rsreg, Temp_temp reg1, Temp_temp reg2) {
    char *inst = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst, "stmfd SP!,{r0-r7}\n");//保护现场
    emit(AS_Oper(inst, NULL, NULL, NULL));

    char *inst2 = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst2, "MOV r0, 's0\n");//传递操作数reg1->r0
    emit(AS_Move(inst2, NULL, L(reg1, NULL)));

    char *inst3 = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst3, "MOV r1, 's0\n");//传递操作数reg2->r1
    emit(AS_Move(inst3, NULL, L(reg2, NULL)));

    char *inst4 = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst4, "bl %s\n", fun);
    emit(AS_Oper(inst4, NULL, NULL, NULL));

    char *inst5 = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst5, "MOV 'd0, r0\n", rsreg);//取回返回值
    emit(AS_Move(inst5, L(rsreg, NULL), NULL));

    char *inst6 = (char *) checked_malloc(sizeof(char) * 120);
    sprintf(inst6, "ldmfd SP!,{r0-r7}\n");//恢复现场
    emit(AS_Oper(inst6, NULL, NULL, NULL));
}

