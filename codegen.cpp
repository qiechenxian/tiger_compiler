//
// Created by zcjsh on 2020/7/21.
//
#include "codegen.h"
TAB_table global_save=NULL;
/// 定义指令字符串长度，可调整大小，节省内存
const int INST_LEN = 40;
int count_func_param = 0;//该全局变量用于调用sys库特殊处理时，判断前四个参数
int args_count = 0;
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



bool Sys_lib_fuc_test(Temp_label lab) {
    return strcmp("getint", Temp_labelString(lab)) == 0 || strcmp("getch", Temp_labelString(lab)) == 0
           || strcmp("getarray", Temp_labelString(lab)) == 0 || strcmp("putint", Temp_labelString(lab)) == 0
           || strcmp("putch", Temp_labelString(lab)) == 0 || strcmp("putarray", Temp_labelString(lab)) == 0
           || strcmp("putf", Temp_labelString(lab)) == 0 || strcmp("starttime", Temp_labelString(lab)) == 0
           || strcmp("stoptime", Temp_labelString(lab)) == 0||strcmp("memset", Temp_labelString(lab)) == 0;
}

// 用于表达式的匹配，每种情形的从句将匹配一个瓦片
static Temp_temp munchExp(T_exp e);

// 用于语句的匹配
static void munchStm(T_stm s, F_frame f);

// 用于过程调用中参数传递到正确位置
static Temp_tempList munchArgs(bool tag, int i, Temp_tempList args);

//调用者保存和恢复
static void munchCallerSave();

static void munchCallerRestore(Temp_tempList tl);

//调用库函数
static void call_lib(c_string fun, Temp_temp rsreg, T_exp exp1, T_exp exp2);

/*
 * 快速匹配没做
 */
AS_instrList F_codegen(F_frame f, T_stmList stmList,TAB_table div_tabel) {
    AS_instrList list;
    T_stmList stmList1;
    global_save=div_tabel;
    // 形参的前四个参数要把寄存器的值保存在栈中
    T_stmList last_stmlist = stmList;
    T_stmList func_label = stmList;
    T_stmList body_list = stmList->tail;

    Temp_temp* callerArray = F_getCallerArray();
    F_accessList formal_access_list = F_getFormals(f);
    for(int argCnt = 0; formal_access_list && (argCnt < 4); formal_access_list = formal_access_list->tail, argCnt ++) {
        Temp_temp temp = F_accessReg(formal_access_list->head);
        T_stm temp_stm = T_Move(T_Temp(temp), T_Temp(callerArray[argCnt]));
        T_stmList temp_stmlist = T_StmList(temp_stm, NULL);

        last_stmlist->tail = temp_stmlist;
        last_stmlist = temp_stmlist;
    }

    last_stmlist->tail = body_list;
    stmList1 = func_label;

    for (; stmList1; stmList1 = stmList1->tail) {
        munchStm(stmList1->head, f);
    }

    if (last && last->head->kind == AS_instr_::I_LABEL) {
        if(F_needReturn(f)) {
            emit(AS_Oper((char *)"NOP\n", NULL, L(F_R0(), NULL), NULL));
        } else {
            emit(AS_Oper((char *)"NOP\n", NULL, NULL, NULL));
        }

    }
    list = iList;
    last = NULL;
    iList = last;
    return list;
}

/*
	循环左移两位
*/
void roundLeftShiftTwoBit(unsigned int &num)
{
    unsigned int overFlow = num & 0xc0000000;//取左移即将溢出的两位
    num = (num << 2) | (overFlow >> 30);//将溢出部分追加到尾部
}

/*
	判断num是否是8位可放的，8位数字循环右移偶数位得到
*/
bool __constExpr(int num)
{
    unsigned int new_num = (unsigned int) num;

    for (int i = 0; i < 16; i++) {

        if (new_num <= 0xff) {
            // 有效表达式
            return true;
        }

        //循环左移2位
        roundLeftShiftTwoBit(new_num);
    }

    return false;
}

/*
	同时处理正数和负数
*/
bool constExpr(int num)
{
    return __constExpr(num) || __constExpr(-num);
}

bool limit_4096(int num){
    if(num>=-4096 && num <= 4095){
        return true;
    }
    return false;
}

static int getShiftTime(int aConst) {
    int times = 1;
    for (int cp = 2; cp<=aConst; cp = cp<<1){
        if (aConst == cp){
            return times;
        }
        times ++;
    }
    return 0;
}

/*
 * MOV ADD SUB CMP AND ORR B BL LDR STR
 */
static Temp_temp munchExp(T_exp e) {
    char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
    char *inst2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
    switch (e->kind) {
        case T_exp_::T_MEM: {
            T_exp mem = e->u.MEM;
            if (mem->kind == T_exp_::T_BINOP) {
                if (mem->u.BINOP.op == T_add && mem->u.BINOP.right->kind == T_exp_::T_CONST) {
                    /* MEM(BINOP(PLUS,e1,CONST(i))) */
                    T_exp e1 = mem->u.BINOP.left;
                    int i = mem->u.BINOP.right->u.CONST;
                    Temp_temp r = Temp_newTemp();
                    Temp_temp r1 = munchExp(e1);
                    if(limit_4096(i)){
                        if (e1->kind == T_exp_::T_TEMP && e1->u.TEMP == F_FP()) {
                            sprintf(inst, "\tldr     'd0, [FP, #%d]\n", i);
                            emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                        } else {
                            sprintf(inst, "\tldr     'd0, ['s0, #%d]\n", i);
                            emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                        }
                    }
                    else{
                        Temp_temp r2 = Temp_newTemp();

                        sprintf(inst,"\tldr     'd0,=%d\n",i);
                        emit(AS_Oper(inst, L(r2, NULL), NULL, NULL));

                        sprintf(inst2, "\tldr     'd0, ['s0, 's1]\n");
                        emit(AS_Oper(inst2,L(r,NULL),L(r1,L(r2,NULL)),NULL));
                    }
                    return r;
                } else if (mem->u.BINOP.op == T_add && mem->u.BINOP.left->kind == T_exp_::T_CONST) {
                    /* MEM(BINOP(PLUS,CONST(i),e1)) */
                    T_exp e1 = mem->u.BINOP.right;
                    int i = mem->u.BINOP.left->u.CONST;
                    Temp_temp r = Temp_newTemp();
                    Temp_temp r1 = munchExp(e1);
                    if(limit_4096(i)){
                        sprintf(inst, "\tldr     'd0, ['s0, #%d]\n", i);
                        emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                    }
                    else{
                        sprintf(inst,"\tldr     'd0,=%d\n",i);
                        emit(AS_Oper(inst, L(r, NULL), NULL, NULL));

                        sprintf(inst2, "\tldr     'd0, ['s0, 'd0]\n");
                        emit(AS_Oper(inst2,L(r,NULL), L(r1, NULL),NULL));
                    }
                    return r;
                } else {
                    /* MEM(e1) */
                    T_exp e1 = mem;
                    Temp_temp r = Temp_newTemp();
                    Temp_temp r1 = munchExp(e1);
                    sprintf(inst, "\tldr     'd0, ['s0]\n");
                    emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                    return r;
                }
            } else if (mem->kind == T_exp_::T_CONST) {
                /* MEM(CONST(i)) */
                int i = mem->u.CONST;
                Temp_temp r = Temp_newTemp();
                //0+const?
                if(limit_4096(i)){
                    sprintf(inst, "\tldr     'd0, [#%d]\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                }
                else{

                    Temp_temp r1 = Temp_newTemp();

                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r1, NULL), NULL, NULL));

                    sprintf(inst2, "\tldr     'd0, ['d1]\n");
                    emit(AS_Oper(inst2, L(r, L(r1,NULL)), NULL, NULL));
                }
                return r;
            } else {
                /* MEM(e1) */
                T_exp e1 = mem;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                sprintf(inst, "\tldr     'd0, ['s0]\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                return r;
            }
        }
        case T_exp_::T_BINOP: {
            if (e->u.BINOP.op == T_add && e->u.BINOP.right->kind == T_exp_::T_CONST)
            {
                /* BINOP(PLUS,e1,CONST(i))  */
                T_exp e1 = e->u.BINOP.left;
                int i = e->u.BINOP.right->u.CONST;
                Temp_temp r = Temp_newTemp();
                if (constExpr(i)) {
                    sprintf(inst, "\tadd     'd0, 's0, #%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                }
                else {
                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));

                    sprintf(inst2, "\tadd     'd0, 's0, 's1\n");
                    emit(AS_Oper(inst2, L(r, NULL), L(munchExp(e1), L(r, NULL)), NULL));
                }
                return r;
            }
            else if(e->u.BINOP.op == T_add && e->u.BINOP.left->kind == T_exp_::T_CONST)
            {
                T_exp e1 = e->u.BINOP.right;
                int i = e->u.BINOP.left->u.CONST;
                Temp_temp r1 = munchExp(e1);
                Temp_temp r = Temp_newTemp();
                if (constExpr(i)) {
                    sprintf(inst, "\tadd     'd0, 's0, #%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                } else {
                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));

                    sprintf(inst2,"\tadd     'd0, 's0, 's1\n"); 
                    emit(AS_Oper(inst2,L(r,NULL),L(r1, L(r, NULL)),NULL));
                }
                return r;
            } else if (e->u.BINOP.op == T_sub && e->u.BINOP.right->kind == T_exp_::T_CONST) {
                /* BINOP(MINUS,e1,CONST(i)) */
                T_exp e1 = e->u.BINOP.left;
                int i = e->u.BINOP.right->u.CONST;
                Temp_temp r1 = munchExp(e1);
                Temp_temp r = Temp_newTemp();
                if (constExpr(i)) {
                    sprintf(inst, "\tsub     'd0, 's0, #%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                } else {
                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));

                    sprintf(inst2,"\tsub     'd0, 's0, 's1\n");
                    emit(AS_Oper(inst2,L(r, NULL),L(r1,L(r, NULL)),NULL));
                }
//                sprintf(inst, "\tsub     'd0, 's0, #%d\n", i);
//                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            }
            else if (e->u.BINOP.op == T_sub && e->u.BINOP.left->kind == T_exp_::T_CONST) {
                /* BINOP(MINUS,CONST(i),e1) */
                T_exp e1 = e->u.BINOP.right;
                int i = e->u.BINOP.left->u.CONST;
                Temp_temp r = Temp_newTemp();
                if (i < 200&&i>=-200) {
                    sprintf(inst, "\tsub     'd0, #%d, 's0\n", i);
                    emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                } else {
                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                    sprintf(inst2,"\tsub     'd0, 's0, 'd0\n");
                    emit(AS_Oper(inst2,L(r,NULL),L(munchExp(e1),NULL),NULL));
                }
//                sprintf(inst, "\tsub     'd0, 's0, #%d\n", i);
//                emit(AS_Oper(inst, L(r, NULL), L(munchExp(e1), NULL), NULL));
                return r;
            }
            else if (e->u.BINOP.op == T_add) {
                /* BINOP(PLUS,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                Temp_temp r1 = munchExp(e1);
                if(e2->kind == T_exp_::T_BINOP && e2->u.BINOP.op == T_mul) {
                    T_exp e2_left = e2->u.BINOP.left;
                    T_exp e2_right = e2->u.BINOP.right;
                    int shift = 0;

                    if(e2_right->kind == T_exp_::T_CONST && (shift = getShiftTime(e2_right->u.CONST))) {
                        sprintf(inst, "\tadd     'd0, 's0, 's1, lsl #%d\n", shift);
                        emit(AS_Oper(inst, L(r, NULL), L(r1, L(munchExp(e2_left), NULL)), NULL));
                    } else {
                        sprintf(inst, "\tmla     'd0, 's0, 's1, 's2\n");
                        emit(AS_Oper(inst, L(r, NULL), L(munchExp(e2_left), L(munchExp(e2_right), L(r1, NULL))), NULL));
                    }
                } else {
                    Temp_temp r2 = munchExp(e2);
                    sprintf(inst, "\tadd     'd0, 's0, 's1\n");
                    emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                }

                return r;
            } else if (e->u.BINOP.op == T_sub) {
                /* BINOP(MINUS,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);
                Temp_temp r = Temp_newTemp();
                sprintf(inst, "\tsub     'd0, 's0, 's1\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else if (e->u.BINOP.op == T_mul) {
                /* BINOP(MUL,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                ///not 2 const
                Temp_temp r1 = nullptr;
                Temp_temp r2 = nullptr;

                if (e1->kind == T_exp_::T_CONST){
                    int i = e1->u.CONST;
                    r2 = munchExp(e2);
                    if (i == 0){
                        sprintf(inst, "\tmov     'd0, #0\n");
                        emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                        return r;
                    } else if (i == 1){
                        sprintf(inst, "\tmov     'd0, 's0\n");
                        emit(AS_Move(inst, L(r, NULL), L(r2, NULL)));
                        return r;
                    }

                    int lshift = getShiftTime(i);
                    if (lshift){
                        sprintf(inst, "\tlsl     'd0, 's0, #%d\n", lshift);
                        emit(AS_Oper(inst, L(r, NULL), L(r2, NULL), NULL));
                        return r;
                    }
                }

                if (e2->kind == T_exp_::T_CONST){
                    int i = e2->u.CONST;
                    r1 = munchExp(e1);
                    if (i == 0){
                        sprintf(inst, "\tmov     'd0, #0\n");
                        emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                        return r;
                    } else if (i == 1){
                        sprintf(inst, "\tmov     'd0, 's0\n");
                        emit(AS_Move(inst, L(r, NULL), L(r1, NULL)));
                        return r;
                    }

                    int lshift = getShiftTime(i);
                    if (lshift){
                        sprintf(inst, "\tlsl     'd0, 's0, #%d\n", lshift);
                        emit(AS_Oper(inst, L(r, NULL), L(r1, NULL), NULL));
                        return r;
                    }
                }

                if(nullptr == r1) r1 = munchExp(e1);
                if(nullptr == r2) r2 = munchExp(e2);
                sprintf(inst, "\tmul     'd0, 's0, 's1\n");
                emit(AS_Oper(inst, L(r, NULL), L(r1, L(r2, NULL)), NULL));
                return r;
            } else if (e->u.BINOP.op == T_div) {
                /* BINOP(DIV,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
                Temp_temp r = Temp_newTemp();
                call_lib((char *)"__aeabi_idiv", r, e1, e2);
                return r;
            } else if (e->u.BINOP.op == T_mod) {
                /* BINOP(MOD,e1,e2) */
                T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;

                if (e2->kind == T_exp_::T_CONST){
                    int shift = getShiftTime(e2->u.CONST);
                    if (shift != 0){
//                        fprintf(stderr, "can option! %d:%d\n", e2->u.CONST, shift);
                        Temp_temp r = Temp_newTemp();
                        Temp_temp r1 = munchExp(e1);
                        sprintf(inst, "\tand 'd0, 's0, #%d\n", e2->u.CONST-1);
                        emit(AS_Oper(inst, L(r, nullptr), L(r1, nullptr), nullptr));
                        return r;
                    }
                }

                Temp_temp r = Temp_newTemp();
                call_lib((char *)"__aeabi_idivmod", r, e1, e2);
                return r;
            } else {
//                EM_error(0, "codegen not munch exp when kind is T_BINOP.");
                EM_errorWithExitCode(-5, 0, "codegen not munch exp when kind is T_BINOP.");
                break;
            }
        }
        case T_exp_::T_CONST: {
            /* CONST(i) */
            int i = e->u.CONST;
            Temp_temp r = Temp_newTemp();
            if(constExpr(i)){
                sprintf(inst, "\tmov     'd0, #%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
            }
            else{
                sprintf(inst, "\tldr     'd0, =%d\n", i);
                emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
            }
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
            sprintf(inst, "\tldr     'd0, =%s\n", Temp_labelString(label));
            emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
            return r;
        }
        case T_exp_::T_CALL:
        case T_exp_::T_ESEQ:
        default: {
//            EM_error(0, "not find munchExp kind.");
            EM_errorWithExitCode(-6, 0, "not find munchExp kind.");
            assert(0);
        }
    }

    // FIXME
    return NULL;
}

static char *funcName(c_string labelString) {
    if (strcmp(labelString, "putf") == 0){
        return (char*)"printf";
    } else if (strcmp(labelString, "starttime") == 0){
        return (char*)"_sysy_starttime";
    } else if (strcmp(labelString, "stoptime") == 0){
        return (char*)"_sysy_stoptime";
    }
    return labelString;
}

/*
 * 使用的指令包括
 */
static void munchStm(T_stm s, F_frame f) {
    char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
    char *inst2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
    char *inst3 = (char *) checked_malloc(sizeof(char) * INST_LEN);
//    char *inst_r0 = (char *) checked_malloc(sizeof(char) * INST_LEN);
//    char *inst_r1 = (char *) checked_malloc(sizeof(char) * INST_LEN);
//    char *inst_r2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
//    char *inst_r3 = (char *) checked_malloc(sizeof(char) * INST_LEN);
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
                    if(limit_4096(i)){
                        sprintf(inst, "\tstr     's0, ['s1, #%d]\n", i);
                        emit(AS_Oper(inst, NULL, L(munchExp(e2), L(munchExp(e1), NULL)), NULL));
                    }
                    else{
                        Temp_temp r1 = Temp_newTemp();
                        sprintf(inst, "\tldr     'd0, =%d\n", i);
                        emit(AS_Oper(inst, L(r1,NULL), NULL, NULL));
                        sprintf(inst2, "\tstr     's0, ['s1, 's2]\n");
                        emit(AS_Oper(inst2, NULL, L(munchExp(e2) ,L(munchExp(e1), L(r1,NULL))), NULL));
                    }
                } else if (dst->u.MEM->kind == T_exp_::T_BINOP
                           && dst->u.MEM->u.BINOP.op == T_add
                           && dst->u.MEM->u.BINOP.left->kind == T_exp_::T_CONST) {
                    /* MOVE(MEM(BINOP(PLUS,CONST(i),e1)),e2) */
                    T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
                    int i = dst->u.MEM->u.BINOP.left->u.CONST;
                    if(limit_4096(i)){
                        sprintf(inst, "\tstr     's0, ['s1, #%d]\n", i);
                        emit(AS_Oper(inst, NULL, L(munchExp(e2), L(munchExp(e1), NULL)), NULL));
                    }
                    else{
                        Temp_temp r1 = Temp_newTemp();
                        sprintf(inst, "\tldr     'd0, =%d\n", i);
                        emit(AS_Oper(inst, L(r1,NULL), NULL, NULL));
                        sprintf(inst2, "\tstr     's0, ['s1, 's2]\n");
                        emit(AS_Oper(inst2, NULL, L(munchExp(e2), L(munchExp(e1) , L(r1,NULL))), NULL));
                    }
                } else if (src->kind == T_exp_::T_MEM) {
                    /* MOVE(MEM(e1), MEM(e2)) */
                    T_exp e1 = dst->u.MEM, e2 = src->u.MEM;
                    Temp_temp r = Temp_newTemp();
                    Temp_temp r1 = munchExp(e1);
                    Temp_temp r2 = munchExp(e2);
                    sprintf(inst, "\tldr     'd0, ['s0]\n");
                    emit(AS_Oper(inst, L(r, NULL), L(r2, NULL), NULL));

                    sprintf(inst2, "\tstr     's0, ['s1]\n");
                    emit(AS_Oper(inst2, NULL, L(r, L(r1, NULL)),  NULL));
                } else if (dst->u.MEM->kind == T_exp_::T_CONST) {
                    /* MOVE(MEM(CONST(i)), e1) */
                    T_exp e1 = src;
                    int i = dst->u.MEM->u.CONST;
                    //TODO 无0寄存器怎么表示0+const?
                    if(limit_4096(i)){
                        sprintf(inst, "\tstr     's0, [#%d]\n", i);
                        emit(AS_Oper(inst, NULL, L(munchExp(e1), NULL), NULL));
                    }
                    else{
                        Temp_temp r1 = Temp_newTemp();
                        sprintf(inst, "\tldr     'd0, =%d\n", i);
                        emit(AS_Oper(inst, L(r1,NULL), NULL, NULL));
                        sprintf(inst2, "\tstr     's0, ['s1]\n");
                        emit(AS_Oper(inst2, NULL,  L(munchExp(e1) , L(r1,NULL)), NULL));
                    }
                } else {
                    /* MOVE(MEM(e1), e2) */
                    T_exp e1 = dst->u.MEM, e2 = src;
                    Temp_temp r1 = munchExp(e1);
                    Temp_temp r2 = munchExp(e2);
                    // 按照真实的操作来说，r2为源（Use），而[r1]应该为结果（define），
                    // 实际的动作含义为把r2保存到r1所代表的内存中，实际上是指针操作，区别于
                    // 一般的算术操作，它会改变r1所指向内存的值
                    sprintf(inst, "\tstr     's0, ['s1]\n");
                    emit(AS_Oper(inst, NULL, L(r2, L(r1, NULL)), NULL));
                }
            } else if (dst->kind == T_exp_::T_TEMP) {
                if (src->kind == T_exp_::T_CALL) {
                    if (src->u.CALL.fun->kind == T_exp_::T_NAME) {
                        /* MOVE(TEMP(t),CALL(NAME(lab),args)) */
                        Temp_label lab = src->u.CALL.fun->u.NAME;
                        T_expList args = src->u.CALL.args;
                        T_expList temp_args = args;
                        int real_arg_cnt = 0;
                        args_count = 0;//用于计算参数个数，静态栈帧不能直接stmfd
                        /// 提前分配一段保存空间
                        Temp_temp t = dst->u.TEMP;
                        count_func_param = 0;

                        Temp_tempList new_args_temp = NULL;
                        for (T_expList arg_list = args; arg_list; arg_list = arg_list->tail) {
                            Temp_temp r;
                            r = munchExp(arg_list->head);
                            new_args_temp = L(r, new_args_temp);
                            real_arg_cnt++;
                        }

                        munchArgs(true, 0, Temp_reverseList(new_args_temp));

                        // 函数调用
                        sprintf(inst, "\tbl      %s\n", Temp_labelString(lab));

                        emit(AS_Oper(inst, F_callersaves(4), F_callersaves(4), AS_Targets(Temp_LabelList(lab, NULL))));

                        sprintf(inst2, "\tmov     'd0, 's0\n");
                        emit(AS_Move(inst2, L(t, NULL), L(F_R0(), NULL)));
                    } else {
                        /* MOVE(TEMP(t),CALL(e1,args)) */
//                        EM_error(0, "call(e1,args) NG");
                        EM_errorWithExitCode(-7, 0, "call(e1,args) NG");
                        assert(0);
//                        T_exp e1 = src->u.CALL.fun;
//                        T_expList args = src->u.CALL.args;
//                        Temp_temp t = dst->u.TEMP;
//                        Temp_temp r1 = munchExp(e1);
//                        Temp_tempList l = munchArgs(false, 0, args);
//                        Temp_tempList calldefs = NULL;
                    }
                } else if(src->kind==T_exp_::T_CONST)
                {/* MOVE(TEMP(i),#const) */
                    int i=src->u.CONST;
                    Temp_temp r=Temp_newTemp();
                    if (constExpr(i))
                    {
                        Temp_temp temp = dst->u.TEMP;
                        sprintf(inst, "\tmov     'd0, #%d\n",i);
                        emit(AS_Oper(inst, L(temp, NULL), NULL,NULL));
                    }
                    else
                    {

                        sprintf(inst, "\tldr     'd0, =%d\n", i);
                        emit(AS_Oper(inst, L(r, NULL), NULL, NULL));
                        Temp_temp temp = dst->u.TEMP;
                        sprintf(inst2, "\tmov     'd0, 's0\n");
                        emit(AS_Move(inst2, L(temp, NULL), L(r, NULL)));
                    }
                } else {
                    /* MOVE(TEMP(i),e1) */
                    T_exp e1 = src;
                    Temp_temp temp = dst->u.TEMP;
                    Temp_temp r1 = munchExp(e1);
                    sprintf(inst, "\tmov     'd0, 's0\n");
                    emit(AS_Move(inst, L(temp, NULL), L(r1, NULL)));
                }
            } else if (dst->kind == T_exp_::T_NAME) {
                //存全局变量
                /* MOVE(NAME(lab),e1) */
                EM_ASSERT_CODE=-59;
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
                sprintf(inst, "\tldr 'd0, =%s\n", Temp_labelString(lab));
                emit(AS_Oper(inst, L(temp, NULL), NULL, NULL));
                sprintf(inst2, "\tstr 's0, ['d0]\n");
                emit(AS_Oper(inst2, L(temp, NULL), L(munchExp(e1), NULL), NULL));
            } else {
                EM_ASSERT_CODE=-60;
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
                    int real_arg_cnt = 0;
                    args_count = 0;//用于计算参数个数，静态栈帧不能直接stmfd
                    count_func_param = 0;

                    Temp_tempList new_args_temp = NULL;
                    for(T_expList arg_list = args; arg_list; arg_list = arg_list->tail) {
                        Temp_temp r = munchExp(arg_list->head);
                        new_args_temp = L(r, new_args_temp);
                        real_arg_cnt ++;
                    }

                    munchArgs(true, 0, Temp_reverseList(new_args_temp));

                    // 函数调用
                    sprintf(inst, "\tbl      %s\n", funcName(Temp_labelString(lab)));
                    emit(AS_Oper(inst, F_callersaves(4), F_callersaves(4), AS_Targets(Temp_LabelList(lab, NULL))));

                } else {
                    /* EXP(CALL(e,args)) */
//                    EM_error(0, "call(e1,args) illegal");
                    EM_errorWithExitCode(-8, 0, "call(e1,args) illegal");
                    assert(0);

//                    T_exp e1 = call->u.CALL.fun;
//                    T_expList args = call->u.CALL.args;
//                    Temp_temp r1 = munchExp(e1);
//                    Temp_tempList l = munchArgs(false, 0, args);
//                    Temp_tempList calldefs = NULL; // TODO
//                    // TODO 此处未写，call(e1,args)语义不明
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
                sprintf(inst, "\tb       %s\n", Temp_labelString(lab));
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
            Temp_label jt = s->u.CJUMP.trues;
            Temp_label jf = s->u.CJUMP.falses;

            if(e2->kind == T_exp_::T_CONST) {
                int i = e2->u.CONST;

                Temp_temp r1 = munchExp(e1);

                if(constExpr(i)){
                    sprintf(inst, "\tcmp     's0, #%d\n", i);
                    emit(AS_Oper(inst, NULL, L(r1, NULL), NULL));
                } else{

                    Temp_temp r = Temp_newTemp();
                    sprintf(inst, "\tldr     'd0, =%d\n", i);
                    emit(AS_Oper(inst, L(r, NULL), NULL, NULL));

                    char *ldr_inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
                    sprintf(ldr_inst, "\tcmp     's0, 's1\n");
                    emit(AS_Oper(ldr_inst, NULL, L(r1, L(r, NULL)), NULL));
                }
            } else {
                Temp_temp r1 = munchExp(e1);
                Temp_temp r2 = munchExp(e2);

                sprintf(inst, "\tcmp     's0, 's1\n");
                emit(AS_Oper(inst, NULL, L(r1, L(r2, NULL)), NULL));
            }

            char *opcode;
            switch (op) {
                case T_eq:
                    opcode = (char *) "beq";
                    break;
                case T_ne:
                    opcode = (char *) "bne";
                    break;
                case T_lt:
                    opcode = (char *) "blt";
                    break;
                case T_gt:
                    opcode = (char *) "bgt";
                    break;
                case T_le:
                    opcode = (char *) "ble";
                    break;
                case T_ge:
                    opcode = (char *) "bge";
                    break;
                default: {
//                    EM_error(0, "not find op.");
                    EM_errorWithExitCode(-9, 0, "not find op.");
                    assert(0);
                }
            }

            sprintf(inst2, "\t%s     %s\n", opcode, Temp_labelString(jt));
            emit(AS_Oper(inst2, NULL, NULL, AS_Targets(Temp_LabelList(jt, NULL))));

            sprintf(inst3, "\tb       %s\n", Temp_labelString(jf));
            emit(AS_Oper(inst3, NULL, NULL, AS_Targets(Temp_LabelList(jf, NULL))));

            break;
        }
        default: {
            EM_ASSERT_CODE=-61;
            assert(0);
        }
    }
}

static Temp_tempList munchArgs(bool tag, int i, Temp_tempList args)
/**
 * 调用过程中传参。
 * \注意 这种传参方法没有使用压栈操作。而是在程序入口处提前分配传参空间。
 * 这样的好处是比压栈的操作效率更高，且无需在函数返回后进行出栈操作。
 * 但需要提前知道 调用函数 中 被调函数需要栈传参的最大空间(一个函数可能调用多个函数)。
 * 这一点可在语义检查阶段加以统计
 *
 * 但arm中栈帧结构还不是很清楚，暂且这样实现。
 * --loyx 2020/7/26
 * @param i 第i个参数
 * @param args
 * @return
 */
{
    if (tag == false) {
        if (args == nullptr) {
            return nullptr;
        }
        char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
        Temp_temp r = args->head;
        if (i)
            sprintf(inst, "\tstr     's0, [SP, #%d]\n", i * get_word_size());
        else
            sprintf(inst, "\tstr     's0, [SP]\n");

        emit(AS_Oper(inst, NULL, L(r, NULL), NULL));

        Temp_tempList old = munchArgs(false, i + 1, args->tail);
        return Temp_TempList(r, old);
    } else {
        count_func_param++;
        if (args == nullptr) {
            return nullptr;
        }
        char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
        Temp_temp r = args->head;
        if (count_func_param > 4) {
            if (i - 4)
                sprintf(inst, "\tstr     's0, [SP, #%d]\n", (i - 4) * get_word_size());
            else
                sprintf(inst, "\tstr     's0, [SP]\n");

            emit(AS_Oper(inst, NULL, L(r,  NULL), NULL));
            Temp_tempList old = munchArgs(true, i + 1, args->tail);
            return Temp_TempList(r, old);
        } else if (args_count >= 0) {
            switch (count_func_param - 1) {
                case 0: {
                    //sprintf(str, "\tstr     's0, ['s1, #%d]\n", (--args_count) * get_word_size());//s0预着色为r0  s1预着色为sp
                    //emit(AS_Oper(str, NULL, L(F_R0(), L(F_SP(), NULL)), NULL));

                    sprintf(inst, "\tmov     'd0, 's0\n");
                    emit(AS_Move(inst, L(F_R0(), NULL), L(r, NULL)));
                    break;
                }
                case 1: {
                    //sprintf(str, "\tstr     's0, ['s1, #%d]\n", (--args_count) * get_word_size());//s0预着色为r1  s1预着色为sp
                    //emit(AS_Oper(str, NULL, L(F_R1(), L(F_SP(), NULL)), NULL));

                    sprintf(inst, "\tmov     'd0, 's0\n");
                    emit(AS_Move(inst, L(F_R1(), NULL), L(r, NULL)));
                    break;
                }
                case 2: {
                    //sprintf(str, "\tstr     's0, ['s1, #%d]\n", (--args_count) * get_word_size());//s0预着色为r2  s1预着色为sp
                    //emit(AS_Oper(str, NULL, L(F_R2(), L(F_SP(), NULL)), NULL));

                    sprintf(inst, "\tmov     'd0, 's0\n");
                    emit(AS_Move(inst, L(F_R2(), NULL), L(r, NULL)));
                    break;
                }
                case 3: {
                    //sprintf(str, "\tstr     's0, ['s1, #%d]\n", (--args_count) * get_word_size());//s0预着色为r3  s1预着色为sp
                    //emit(AS_Oper(str, NULL, L(F_R3(), L(F_SP(), NULL)), NULL));

                    sprintf(inst, "\tmov     'd0, 's0\n");
                    emit(AS_Move(inst, L(F_R3(), NULL), L(r, NULL)));
                    break;
                }
            }
            Temp_tempList old = munchArgs(true, i + 1, args->tail);
            return Temp_TempList(r, old);
            //sprintf(inst, "\tldr     'd0, ['s0, #%d]\n", i);
        }
    }

    // FIXME
    return nullptr;
}

/*
 * 调用库函数 resReg=fun(arg1,arg2);
 */
static void call_lib(c_string fun, Temp_temp rsreg, T_exp exp1, T_exp exp2) {//todo 确认是否需要修改
//    char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
//    sprintf(inst, "\tstmfd   sp!, {r0-r1}\n");//保护现场
//    emit(AS_Oper(inst, NULL, NULL, NULL));

//    Temp_temp* callerReg = F_getCallerArray();
//    Temp_tempList caller2List = L(callerReg[0], L(callerReg[1], NULL));
    AS_instrList save_div=NULL;
    AS_instr instr=NULL,instr1=NULL,instr2=NULL,instr3=NULL,instr4=NULL,instr5=NULL;
    Temp_temp ret_reg;
    Temp_temp reg1,reg2;
    bool imm_tag= false;
    int sit=0;
    int save_number;
    if (strcmp(fun, "__aeabi_idiv") == 0) {
        ret_reg = F_R0();
    } else if (strcmp(fun, "__aeabi_idivmod") == 0) {
        ret_reg = F_R1();
    } else {
        EM_ASSERT_CODE=-62;
        assert("error from call_lib in codegen.cpp ");
    }

    T_exp src=exp1;
    T_exp srcl=exp2;
    if(src->kind!=T_exp_::T_CONST)
    {
        reg1=munchExp(src);
    }
    if(srcl->kind!=T_exp_::T_CONST)
    {
        reg2=munchExp(srcl);
    }


    if(src->kind==T_exp_::T_CONST)
    {/* MOVE(TEMP(i),#const) */
        int i=src->u.CONST;
        Temp_temp r=Temp_newTemp();
        if (constExpr(i))
        {
            char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst, "\tmov     'd0, #%d\n",i);
            instr=AS_Oper(inst, L(F_R0(),NULL), NULL, NULL);
            emit(instr);
        }
        else
        {
            char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst, "\tldr     'd0, =%d\n", i);
            instr=AS_Oper(inst, L(r, NULL), NULL, NULL,i);
            emit(instr);
            char *inst1 = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst1, "\tmov     'd0, 's0\n");
            instr1=AS_Move(inst1, L(F_R0(),NULL), L(r, NULL));
            emit(instr1);
        }
        imm_tag=true;
        sit=1;
        save_number=i;
    }
    else
    {
        char *inst = (char *) checked_malloc(sizeof(char) * INST_LEN);
        sprintf(inst, "\tmov     'd0, 's0\n");//传递操作数reg1->r0
        instr=AS_Move(inst, L(F_R0(),NULL), L(reg1, NULL));
        emit(instr);
    }

    if(srcl->kind==T_exp_::T_CONST)
    {/* MOVE(TEMP(i),#const) */
        int i=srcl->u.CONST;
        Temp_temp r=Temp_newTemp();
        if (constExpr(i))
        {
            char *inst2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst2, "\tmov     'd0, #%d\n",i);
            instr2=AS_Oper(inst2, L(F_R1(),NULL), NULL, NULL);
            emit(instr2);
        }
        else
        {
            char *inst2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst2, "\tldr     'd0, =%d\n", i);
            instr2=AS_Oper(inst2, L(r, NULL), NULL, NULL);
            emit(instr2);
            char *inst3 = (char *) checked_malloc(sizeof(char) * INST_LEN);
            sprintf(inst3, "\tmov     'd0, 's0\n");
            instr3=AS_Move(inst3, L(F_R1(),NULL), L(r, NULL));
            emit(instr3);
        }
        imm_tag=true;
        sit=2;
        save_number=i;
    }
    else
    {
        char *inst2 = (char *) checked_malloc(sizeof(char) * INST_LEN);
        sprintf(inst2, "\tmov     'd0, 's0\n");//传递操作数reg2->r1
        instr2=AS_Move(inst2, L(F_R1(),NULL), L(reg2, NULL));
        emit(instr2);
    }
    char *inst4 = (char *) checked_malloc(sizeof(char) * INST_LEN);
    sprintf(inst4, "\tbl      %s\n", fun);
    if(imm_tag==true&&sit==1)
    {
        Temp_temp save_t= Temp_new_imm_Temp(save_number);
        instr4=AS_Oper(inst4,L(save_t,L(reg2,L(rsreg, NULL))), F_callersaves(4), NULL, true);
        emit(instr4);//这里的R1和R2仅仅用于代表除数或者被除数信息，并无去他意义
    }
    else if(imm_tag==true&&sit==2)
    {
        Temp_temp save_t= Temp_new_imm_Temp(save_number);
        instr4=AS_Oper(inst4,L(reg1,L(save_t,L(rsreg, NULL))), F_callersaves(4), NULL, true);
        emit(instr4);
    } else
    {
        instr4=AS_Oper(inst4,L(reg1,L(reg2,L(rsreg, NULL))), F_callersaves(4), NULL, false);
        emit(instr4);
    }

    char *inst5 = (char *) checked_malloc(sizeof(char) * INST_LEN);
    sprintf(inst5, "\tmov     'd0, 's0\n");//取回返回值
    instr5=AS_Move(inst5, L(rsreg, NULL), L(ret_reg, NULL), true);//该语句的def被dataflow忽略
    emit(instr5);
    if(instr!=NULL)
    {save_div=AS_InstrList(instr,save_div);}
    if(instr1!=NULL)
    {save_div=AS_InstrList(instr1,save_div);}
    if(instr2!=NULL)
    {save_div=AS_InstrList(instr2,save_div);}
    if(instr3!=NULL)
    {save_div=AS_InstrList(instr3,save_div);}
    if(instr5!=NULL)
    {save_div=AS_InstrList(instr5,save_div);}
    TAB_enter(global_save,instr4,save_div);
}

