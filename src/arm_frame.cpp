//
// Created by loyx on 2020/5/10.
//
#include "frame.h"
#include "assem.h"
#include "temp.h"

const int F_WORD_SIZE = 4; /// 32位机器
static const int F_K = 4; /// 保存在Reg中参数的数量(待定)


static Temp_temp r0 = NULL, r1 = NULL, r2 = NULL, r3 = NULL;
static Temp_temp r4 = NULL, r5 = NULL, r6 = NULL, r7 = NULL, r8 = NULL, r9 = NULL, r10 = NULL;

static Temp_temp fp = NULL;//r11, 帧指针
static Temp_temp ip = NULL;//r12，子程序间scratch
static Temp_temp sp = NULL;//r13，栈指针
static Temp_temp lr = NULL;//r14,连接寄存器，用于保存子程序返回地址
static Temp_temp pc = NULL;//r15，程序计数器
static Temp_temp zero = nullptr;
static Temp_temp fr = nullptr;
//static Temp_temp ra = nullptr; //mips中的ra相当于arm的lr
//static Temp_temp rv = nullptr; //帧指针

static Temp_tempList registers = NULL;
static Temp_tempList specialregs = NULL;


/*
 * 寄存器结构
 */
// 初始化
void F_initRegisters(void) {

    r0 = Temp_new_special((char*)"R0", 0);
    r1 = Temp_new_special((char*)"R1", 1);
    r2 = Temp_new_special((char*)"R2", 2);
    r3 = Temp_new_special((char*)"R3", 3);
    r4 = Temp_new_special((char*)"R4", 4);
    r5 = Temp_new_special((char*)"R5", 5);
    r6 = Temp_new_special((char*)"R6", 6);
    r7 = Temp_new_special((char*)"R7", 7);
    r8 = Temp_new_special((char*)"R8", 8);
    r9 = Temp_new_special((char*)"R9", 9);
    r10 = Temp_new_special((char*)"R10", 10);

#if 1
    fp = Temp_new_special((char*)"FP", 11);
    sp = Temp_new_special((char*)"SP", 12);
    ip = Temp_new_special((char*)"IP", 13);
    lr = Temp_new_special((char*)"LR", 14);
    pc = Temp_new_special((char*)"PC", 15);
#else
    fp = Temp_newTemp();
    sp = Temp_newTemp();
    ip = Temp_newTemp();
    lr = Temp_newTemp();
    pc = Temp_newTemp();
#endif

    specialregs = Temp_TempList(fp,
                                Temp_TempList(sp,
                                              Temp_TempList(lr, NULL)));
}
Temp_map F_initialRegisters(F_frame f){
    Temp_map m = get_frame_precored_map(f);

    Temp_enter(m, fp, (char*)"FP");
    Temp_enter(m, sp, (char*)"SP");
    Temp_enter(m, lr, (char*)"LR");
//    Temp_enter(m, ip, (char*)"IP");
//    Temp_enter(m, pc, (char*)"PC");

    Temp_enter(m, r0, (char*)"R0");
    Temp_enter(m, r1, (char*)"R1");
    Temp_enter(m, r2, (char*)"R2");
    Temp_enter(m, r3, (char*)"R3");
    Temp_enter(m, r4, (char*)"R4");
    Temp_enter(m, r5, (char*)"R5");
    Temp_enter(m, r6, (char*)"R6");
    Temp_enter(m, r7, (char*)"R7");
    Temp_enter(m, r8, (char*)"R8");
    Temp_enter(m, r9, (char*)"R9");
    Temp_enter(m, r10, (char*)"R10");

    return m;
}
Temp_temp F_FP()//取帧指针
{
    if (fp == NULL) {
        F_initRegisters();
    }
    return fp;
}
//TODO 取栈指针，为什么两个？
Temp_temp F_RV(void){
    if (r0 == NULL) {
        F_initRegisters();
    }
    return r0;
}

Temp_temp F_SP()//取栈指针
{
    if (sp == NULL) {
        F_initRegisters();
    }
    return sp;
}

Temp_temp F_IP()//取栈指针
{
    if (ip == NULL) {
        F_initRegisters();
    }
    return ip;
}

Temp_temp F_PC() //取pc
{
    if (pc == nullptr) {
        F_initRegisters();
    }
    return pc;
}

Temp_temp F_LR() {
    if (lr == NULL) {
        F_initRegisters();
    }
    return lr;
}

Temp_temp F_ZERO(void) {
    if (zero == nullptr) {
        zero = Temp_newTemp();
    }
    return zero;
}

Temp_temp F_R0()
{
    if (r0 == nullptr) {
        F_initRegisters();
    }
    return r0;
}

Temp_temp F_R8()
{
    if (r8 == nullptr) {
        F_initRegisters();
    }
    return r8;
}

Temp_temp F_R9()
{
    if (r9 == nullptr) {
        F_initRegisters();
    }
    return r9;
}

Temp_temp F_R10()
{
    if (r10 == nullptr) {
        F_initRegisters();
    }
    return r10;
}


Temp_temp F_R1()
{
    if (r1 == nullptr){
        F_initRegisters();
    }
    return r1;
}

Temp_temp F_R2()
{
    if (r2 == nullptr){
        F_initRegisters();
    }
    return r2;
}

Temp_temp F_R3()
{
    if (r3 == nullptr)
    {
        F_initRegisters();
    }
    return r3;
}

Temp_temp F_R4()
{
    if (r4 == nullptr) {
        F_initRegisters();
    }
    return r4;
}

Temp_temp F_R5()
{
    if (r5 == nullptr) {
        F_initRegisters();
    }
    return r5;
}

Temp_temp F_R6()
{
    if (r6 == nullptr) {
        F_initRegisters();
    }
    return r6;
}

Temp_temp F_R7()
{
    if (r7 == nullptr) {
        F_initRegisters();
    }
    return r7;
}

 Temp_tempList L(Temp_temp h, Temp_tempList t) {
    return Temp_TempList(h, t);
}

//TODO 需添加剩余寄存器,是否需要fp，sp
Temp_tempList F_registers(void) {
    if (fp == NULL) {
        F_initRegisters();
    }

    return L(r0, L(r1, L(r2, L(r3, L(r4, L(r5, L(r6, L(r7, L(r8, L(r9, NULL))))))))));
}

//TODO 调用者保护寄存器
Temp_tempList F_callersaves(int real_arg_cnt) {
    if (fp == NULL) {
        F_initRegisters();
    }

    Temp_tempList callersave = NULL;
    Temp_temp * callerRegs = F_getCallerArray();
    for(int k = 0; k < real_arg_cnt && k < 4; k ++) {
        callersave = L(callerRegs[k], callersave);
    }

    return callersave;
}

Temp_temp* callerArray = nullptr;

Temp_temp* F_getCallerArray()
{
    if (!callerArray){
        callerArray = (Temp_temp*)checked_malloc(10*sizeof(Temp_temp));
        callerArray[0] = F_R0();
        callerArray[1] = F_R1();
        callerArray[2] = F_R2();
        callerArray[3] = F_R3();
        callerArray[4] = F_R4();
        callerArray[5] = F_R5();
        callerArray[6] = F_R6();
        callerArray[7] = F_R7();
        callerArray[8] = F_R8();
        callerArray[9] = F_R9();
    }

    return callerArray;
}

Temp_temp F_getCallerArrayByIndex(int index)
{
    Temp_temp* array = F_getCallerArray();
    return array[index];
}


//TODO
Temp_tempList F_calleesaves(void) {
    if (fp == NULL) {
        F_initRegisters();
    }
    return Temp_TempList(NULL, NULL);
}

/** class declare */
struct F_access_ {
    enum {
        inFrame, inReg, inGlobal
    } kind;
    struct {
        int offset;
        Temp_temp reg;
        Temp_label global;
    } u;
};

struct F_frame_ {
    Temp_label name;
    Temp_label func_done_label;
    F_accessList formals;
    F_accessList locals;
    int formals_count; // 参数个数
    int local_count;
    int callee_max_args;
    int temp_space;  /// todo 为寄存器分配后，保存临时变量空间预留的接口
    bool isLeaf;
    bool needReturn; // 需要有返回值
    Temp_map precoredMap;

    /** instructions required view shift*/
};//添加局部变量域




/** function prototype */
static F_access InFrame(int offset);

static F_access InReg(Temp_temp reg, int num);

static F_accessList F_AccessList(F_access head, F_accessList tail);

static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals);


/** 构造函数 */
static F_access InFrame(int offset) {
    F_access fa = (F_access) checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inFrame;
    fa->u.offset = offset;
    return fa;
}

static F_access InReg(Temp_temp reg,int temp) {
    F_access fa = (F_access) checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inReg;
    fa->u.offset=temp;
    fa->u.reg = reg;
    return fa;
}

static F_access InGlobal(Temp_label global) {
    auto fa = (F_access) checked_malloc(sizeof(F_access_));
    fa->kind = F_access_::inGlobal;
    fa->u.global = global;
    return fa;
}

static F_accessList F_AccessList(F_access head, F_accessList tail) {
    F_accessList p = (F_accessList) checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

Temp_label F_getGlobalLabel(F_access fa) {
    /**
     * 为translate中的Tr_getGlobalLabel提供底层实现
     */
    EM_ASSERT_CODE=-57;
    assert(fa->kind == F_access_::inGlobal);
    return fa->u.global;
}

/** 辅助函数 */
static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals, int & arg_cnt)
/**
 * 根据escape分配形参access，目前escape全为true，即所有参数都不是必须要放到栈中
 *
 * 在老师配置的arm机器中，参数的放置位置如下：
 *      以调用int foo(int a0, int a1, int a3, int a4, int a5, int a6, int a7)为例
 *       旧FP-> |               |
 *              |      ...      |
 *              |       a7      |
 *              |       a6      |
 *              |       a5      |
 *              |       a4      |
 *              |       a3      |
 *              |       a2      |
 *              |       a1      |
 *        FP->  |   旧LR,FP等    |
 *              |   局部变量等    |
 *              |      ...      |
 *         SP-> |               |
 * @param frame 当前函数栈帧
 * @param formals 形参escape信息
 * @return 形参accessList
 */
{
    F_accessList access_list = nullptr, list_tail = nullptr;
    int args_inReg_cnt = 0;
    int args_inFrame_cnt = 1;
    arg_cnt = 0;
    for (U_boolList iter = formals; iter; iter = iter->tail) {
        arg_cnt ++;

        F_access access = nullptr;
        if (args_inReg_cnt < F_K && (iter->head == true)) {
            //暂时采取全部放在堆栈的存储方式,之后进行寄存器分配优化时修改
            access = InReg(Temp_newTemp(), F_WORD_SIZE * (-frame->local_count));
            args_inReg_cnt++;
        } else {
            access = InFrame(args_inFrame_cnt++ * F_WORD_SIZE);
        }
        if (access_list) {
            list_tail->tail = F_AccessList(access, nullptr);
            list_tail = list_tail->tail;
        } else {
            access_list = F_AccessList(access, nullptr);
            list_tail = access_list;
        }
    }

    return access_list;
}


/** frame 相关 */
void F_setFrameCalleeArgs(F_frame frame, int callee_args) {
    if (strcmp(S_getName(frame->name), "main") == 0){
        frame->isLeaf = false;
    } else{
        frame->isLeaf = callee_args == -1;
    }
    if (callee_args > frame->callee_max_args)
        frame->callee_max_args = callee_args;
}

// R4~R7
#define STACK_PROTECT_REG_NUM_MAX 6
#define STACK_PROTECT_REG_NUM_MAX_MAIN 6
#define CALL_LIB_PROTECED_REG_NUM 0

F_frame F_newFrame(Temp_label name, U_boolList formals, bool needReturn) {
    F_frame f = (F_frame) checked_malloc(sizeof(*f));
    f->name = name;
    f->local_count = STACK_PROTECT_REG_NUM_MAX + 1 + CALL_LIB_PROTECED_REG_NUM;
    f->formals = makeFormalAccessList(f, formals, f->formals_count);
    f->locals = nullptr;
    f->isLeaf = true;
    f->needReturn = needReturn;
    f->temp_space = 0;
    f->callee_max_args = -1;
    f->func_done_label=Temp_newLabel();
    f->precoredMap = Temp_empty();;
    return f;
}

Temp_label F_getName(F_frame frame) {
    return frame->name;
}

bool F_needReturn(F_frame frame) {
    return frame->needReturn;
}

Temp_map get_frame_precored_map(F_frame frame)
{
    return frame->precoredMap;
}

F_accessList F_getFormals(F_frame frame) {
    return frame->formals;
}

int F_getFormalsCount(F_frame frame) {
    return frame->formals_count;
}

int F_accessOffset(F_access a) {

    if (a->kind != F_access_::inFrame) {
//        EM_error(0, "Offset of a reg access is invalid");
    }

    return a->u.offset;
}

bool F_accessIsReg(F_access a) {
    return a->kind == F_access_::inReg;
}

Temp_temp F_accessReg(F_access a) {
    if (a->kind != F_access_::inReg) {
//        EM_error(0, "Reg of a frame access is invalid");
        EM_errorWithExitCode(-3, 0, "Reg of a frame access is invalid");
    }

    return a->u.reg;
}

F_access F_allocLocal(F_frame frame, bool escape, int size) {
    frame->local_count += size;
    F_access access;
    if(escape) {
        access = InFrame(F_WORD_SIZE * (-frame->local_count));
        frame->locals = F_AccessList(access, frame->locals);
    } else {
        access =InReg(Temp_newTemp(),F_WORD_SIZE * (-frame->local_count));
        frame->locals = F_AccessList(access, frame->locals);
    }

    return access;
}

F_access F_allocGlobal(S_symbol global) {
    /**
     * 仅返回一个全局变量的label
     */
    return InGlobal((Temp_label) global);
}

T_exp F_Exp(F_access acc, T_exp framePtr)//将F_access转换为tree表达式
{
    switch (acc->kind) {
        case F_access_::inFrame:
            return F_expWithIndex(acc, framePtr, 0);
        case F_access_::inReg:
            return T_Temp(acc->u.reg);
        case F_access_::inGlobal:
            return T_Mem(T_Name(acc->u.global)); /// 对于全局变量非数组返回T_Name
    }
}

T_exp F_expWithIndex(F_access acc, T_exp framePtr, int index)
/**
 * 该函数是为了访问栈中数组的index处的地址，index是数组拉平后的索引
 * \注意 !!!此函数只在初始化时使用!!!
 * @param acc 数组的基址
 * @param framePtr 栈基址
 * @param index 数组拉平后的索引
 * @return T_Mem() 栈中 基址+offset+index 的位置
 */
{
    return T_Mem(T_Binop(T_add, framePtr, T_Const(get_word_size() * index + acc->u.offset)));
}
T_exp F_fp_address(F_access acc,int index)
{
    Temp_temp get_fp=F_FP();
    T_exp framePtr=T_Temp(get_fp);
    return T_Binop(T_add, framePtr, T_Const(get_word_size() * index + acc->u.offset));
}

int get_word_size() {
    return F_WORD_SIZE;
}

F_accessList F_formals(F_frame f) {
    return f->formals;
}

/// 片段相关F_frag
F_fragList F_FragList(F_frag head, F_fragList tail) {
    F_fragList new_frag_list = (F_fragList) checked_malloc(sizeof(*new_frag_list));
    new_frag_list->head = head;
    new_frag_list->tail = tail;
    return new_frag_list;
}

F_frag F_StringFrag(Temp_label label, c_string str) {
    F_frag new_frag = (F_frag) checked_malloc(sizeof(*new_frag));
    new_frag->kind = F_frag_::F_stringFrag;
    new_frag->u.stringg.str = str;
    new_frag->u.stringg.label = label;
    return new_frag;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
    F_frag new_frag = (F_frag) checked_malloc(sizeof(*new_frag));
    new_frag->kind = F_frag_::F_procFrag;
    new_frag->u.proc.body = body;
    new_frag->u.proc.frame = frame;
    return new_frag;
}

F_frag F_GlobalFrag(Temp_label label, int size, U_pairList init_values, bool comm) {
    auto new_frag = (F_frag) checked_malloc(sizeof(F_frag_));
    new_frag->kind = F_frag_::F_globalFrag;
    new_frag->u.global.label = label;
    new_frag->u.global.size = size;
    new_frag->u.global.init_values = init_values;
    new_frag->u.global.comm = comm;
    return new_frag;
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) {
    return stm;//中间代码阶段的虚实现
}

static Temp_tempList returnSink = nullptr;

/*
 * 告诉寄存器分分配时过程的出口那些寄存器是活跃的，例如临时变量0，返回地址，栈指针，调用者保护的寄存器
 */
AS_instrList F_procEntryExit2(AS_instrList body) {
    Temp_tempList calleeSaves = nullptr;
    if (!returnSink)
        returnSink = Temp_TempList(F_LR(),
                        Temp_TempList(F_FP(),
                           Temp_TempList(F_SP(), calleeSaves)));
    return AS_splice(body, AS_InstrList(
            AS_Oper((char*)"", nullptr, returnSink, nullptr), nullptr));
}

static int getSpace(F_frame frame)
{
    int local_space = 0;
//    for (F_accessList iter = frame->locals; iter; iter = iter->tail){
//        if (iter->head->kind == F_access_::inFrame){
//            local_space += 1;
//        }
//    }
    local_space = frame->local_count;

    int callee_space = 0;
    if (frame->callee_max_args > 0)
        callee_space = frame->callee_max_args;

    return local_space + callee_space + frame->temp_space;
//    return local_space + frame->temp_space;
}

/*
 *
 */
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
    const int INST_SIZE = 40;
    AS_instrList head_inst_list = NULL, tail_inst_list = NULL;
    AS_instrList temp_inst_list;
    char * tempinst_buf;
    int word_size = get_word_size();
    char *name = Temp_labelString(frame->name);

    int recover_offset;
    if (0 == strcmp(name, "main")) {
        recover_offset = (1 + STACK_PROTECT_REG_NUM_MAX_MAIN) * word_size;
    } else {
        recover_offset = (1 + STACK_PROTECT_REG_NUM_MAX) * word_size;
    }

    /** 函数入口label*/
    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    snprintf(tempinst_buf, INST_SIZE, "%s:\n", name);
    temp_inst_list = AS_InstrList(AS_Label(tempinst_buf, frame->name), NULL);
    head_inst_list = AS_splice(head_inst_list, temp_inst_list);

    /** 函数入口处的汇编指令 */
    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    if (strcmp(name, "main")) {
        snprintf(tempinst_buf, INST_SIZE, "\tstmfd   SP!, {R4-R9, FP, LR}\n");
    } else {
        snprintf(tempinst_buf, INST_SIZE, "\tstmfd   SP!, {R4-R9, FP, LR}\n");
    }
    temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL),NULL);
    head_inst_list = AS_splice(head_inst_list, temp_inst_list);

    // TODO recover_offset可能超过立即数的限制，需要先读都寄存器，然后再加法
    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    sprintf(tempinst_buf, "\tadd     FP, SP, #%d\n", recover_offset);
    temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
    head_inst_list = AS_splice(head_inst_list, temp_inst_list);

    int space = getSpace(frame);
    if (space > 0){

        int tmp_wsize = space * word_size;

        if (constExpr(tmp_wsize)){
            tempinst_buf = (char*)checked_malloc(sizeof(char) * INST_SIZE);
            snprintf(tempinst_buf, INST_SIZE, "\tsub     SP, SP, #%d\n", tmp_wsize);
            temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
            head_inst_list = AS_splice(head_inst_list, temp_inst_list);
        } else{
            tempinst_buf = (char*)checked_malloc(sizeof(char ) * INST_SIZE);
            snprintf(tempinst_buf, INST_SIZE, "\tldr     R4, =%d\n", tmp_wsize);
            temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
            head_inst_list = AS_splice(head_inst_list, temp_inst_list);

            tempinst_buf = (char*)checked_malloc(sizeof(char) * INST_SIZE);
            sprintf(tempinst_buf, "\tsub     SP, SP, R4\n");
            temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
            head_inst_list = AS_splice(head_inst_list, temp_inst_list);
        }
    }

    /** 将函数入口指令和body指令连接 */
    head_inst_list = AS_splice(head_inst_list, body->tail);

    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    snprintf(tempinst_buf, INST_SIZE, "\tsub     SP, FP, #%d\n", recover_offset);
    temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
    tail_inst_list = AS_splice(tail_inst_list, temp_inst_list);

    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    if (strcmp(name, "main")) {
        snprintf(tempinst_buf, INST_SIZE, "\tldmfd   SP!, {R4-R9, FP, LR}\n");
    } else {
        snprintf(tempinst_buf, INST_SIZE, "\tldmfd   SP!, {R4-R9, FP, LR}\n");
    }
    temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
    tail_inst_list = AS_splice(tail_inst_list, temp_inst_list);

    tempinst_buf = (char *) checked_malloc(sizeof(char) * INST_SIZE);
    snprintf(tempinst_buf, INST_SIZE, "\tbx      LR\n");
    temp_inst_list = AS_InstrList(AS_Oper(tempinst_buf, NULL, NULL, NULL), NULL);
    tail_inst_list = AS_splice(tail_inst_list, temp_inst_list);

    /** 连接上出口指令 */
    AS_instrList total_inst = AS_splice(head_inst_list, tail_inst_list);

    char *entry_meta = (char *) checked_malloc(sizeof(char) * INST_SIZE*3);
    snprintf(entry_meta, INST_SIZE*3, "\t.align  2\n\t.global %s\n\t.type   %s, %%function\n", name, name);

    char *exit_meta = (char *) checked_malloc(sizeof(char) * INST_SIZE*3);
    snprintf(exit_meta, INST_SIZE*3, "\t.size   %s, .-%s", name, name);

    return AS_Proc(entry_meta, total_inst, exit_meta);
}
Temp_label get_done_label(F_frame f_frame)
{
    return f_frame->func_done_label;
}

Temp_map F_tempMap = nullptr;
int get_offset(F_access g_access)
{
    return g_access->u.offset;
}

void F_setMemArgs(F_frame frame)
{
    if (3 > frame->callee_max_args){
        frame->callee_max_args = 3;
        frame->isLeaf = false;
    }
}

F_access look_for_f_offset(Temp_temp temp,F_frame f)
{
    F_accessList temp_access=f->locals;

    for(;temp_access;temp_access=temp_access->tail) {
        if((temp_access->head->kind == F_access_::inReg) &&
            temp_access->head->u.reg->num == temp->num) {
            return temp_access->head;
        }
    }

    return NULL;
}
