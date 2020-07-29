//
// Created by loyx on 2020/5/10.
//
#include "frame.h"
#include "assem.h"

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

    r0 = Temp_new_special("R0");
    r1 = Temp_new_special("R1");
    r2 = Temp_new_special("R2");
    r3 = Temp_new_special("R3");
    r4 = Temp_new_special("R4");
    r5 = Temp_new_special("R5");
    r6 = Temp_new_special("R6");
    r7 = Temp_new_special("R7");
    r8 = Temp_new_special("R8");
    r9 = Temp_new_special("R9");
    r10 = Temp_new_special("R10");

    fp = Temp_new_special("FP");
    sp = Temp_new_special("SP");
    ip = Temp_new_special("IP");
    lr = Temp_new_special("LR");
    pc = Temp_new_special("PC");

    specialregs = Temp_TempList(fp,
                                Temp_TempList(sp,
                                              Temp_TempList(lr, NULL)));

}
Temp_map F_initialRegisters(F_frame f){
    Temp_map m=Temp_empty();

    Temp_enter(m, fp, "FP");
    Temp_enter(m, sp, "SP");
    Temp_enter(m, lr, "LR");
//    Temp_enter(m, ip, "IP");
//    Temp_enter(m, pc, "PC");

    Temp_enter(m, r0, "R0");
    Temp_enter(m, r1, "R1");
    Temp_enter(m, r2, "R2");
    Temp_enter(m, r3, "R3");
    Temp_enter(m, r4, "R4");
    Temp_enter(m, r5, "R5");
    Temp_enter(m, r6, "R6");
    Temp_enter(m, r7, "R7");
    Temp_enter(m, r8, "R8");
    Temp_enter(m, r9, "R9");
    Temp_enter(m, r10, "R10");

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
    if (fp == NULL) {
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
        char save[5];
        sprintf(save, "%s", "PC");
        pc = Temp_new_special(save);
    }
    return pc;
}

Temp_temp F_LR() {
    if (lr == NULL) {
        F_initRegisters();
    }
    return lr;
}

Temp_temp F_R(c_string save) {
    fr = Temp_new_special(save);
    return fr;
}

Temp_temp F_ZERO(void) {
    if (zero == nullptr) {
        zero = Temp_newTemp();
    }
    return zero;
}

//TODO 需添加剩余寄存器
Temp_tempList F_registers(void) {
    if (fp == NULL) {
        F_initRegisters();
    }
    return Temp_TempList(r0,
                         Temp_TempList(r1,
                                       Temp_TempList(r2,
                                                     Temp_TempList(r3, NULL))));
}

//TODO 调用者保护寄存器
Temp_tempList F_callersaves(void) {
    if (fp == NULL) {
        F_initRegisters();
    }
    return Temp_TempList(NULL, NULL);
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
    F_accessList formals;
    F_accessList locals;
    int local_count;
    int callee_max_args;
    int temp_space;  /// todo 为寄存器分配后，保存临时变量空间预留的接口
    bool isLeaf;
    /** instructions required view shift*/
};//添加局部变量域



/** function prototype */
static F_access InFrame(int offset);

static F_access InReg(Temp_temp reg);

static F_accessList F_AccessList(F_access head, F_accessList tail);

static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals);


/** 构造函数 */
static F_access InFrame(int offset) {
    F_access fa = (F_access) checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inFrame;
    fa->u.offset = offset;
    return fa;
}

static F_access InReg(Temp_temp reg) {
    F_access fa = (F_access) checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inReg;
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
    assert(fa->kind == F_access_::inGlobal);
    return fa->u.global;
}

/** 辅助函数 */
static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals)
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
    for (U_boolList iter = formals; iter; iter = iter->tail) {
        F_access access = nullptr;
        if (args_inReg_cnt <= F_K && (iter->head == false) && false) {//暂时采取全部放在堆栈的存储方式,之后进行寄存器分配优化时修改
            access = InReg(Temp_newTemp());
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
    frame->callee_max_args = callee_args;
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
    F_frame f = (F_frame) checked_malloc(sizeof(*f));
    f->name = name;
    f->formals = makeFormalAccessList(f, formals);
    f->local_count = 1; ///为保存旧FP预留空间 todo 当该函数为子叶函数时，可优化掉栈帧 --loyx 2020/7/25
    f->locals = nullptr;
    f->isLeaf = true;
    f->temp_space = 0;
    f->callee_max_args = -1;
    return f;
}

Temp_label F_getName(F_frame frame) {
    return frame->name;
}

F_accessList F_getFormals(F_frame frame) {
    return frame->formals;
}

int F_accessOffset(F_access a) {
    if (a->kind != F_access_::inFrame) {
        EM_error(0, "Offset of a reg access is invalid");
    }

    return a->u.offset;
}

Temp_temp F_accessReg(F_access a) {
    if (a->kind != F_access_::inReg) {
        EM_error(0, "Reg of a frame access is invalid");
    }

    return a->u.reg;
}

F_access F_allocLocal(F_frame frame, bool escape, int size) {
    frame->local_count += size;
    if (escape) {
        F_access access = InFrame(F_WORD_SIZE * (-frame->local_count));
        frame->locals = F_AccessList(access, frame->locals);
        return access;
    } else {
        return InReg(Temp_newTemp());
    }
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
        returnSink = Temp_TempList(F_ZERO(),
                                   Temp_TempList(F_LR(),
                                                 Temp_TempList(F_SP(), calleeSaves)));
    return AS_splice(body, AS_InstrList(
            AS_Oper("", nullptr, returnSink, nullptr), nullptr));
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
}

/*
 *
 */
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
    AS_instrList head_inst_list, tail_inst_list;
    AS_instrList head_inst_ptr, tail_inst_ptr;
    int word_size = get_word_size();
    char *name = Temp_labelString(frame->name);

    /** 函数入口label*/
    char *fun_label = (char *) checked_malloc(sizeof(char) * 20);
    sprintf(fun_label, "%s:\n", name);
    head_inst_list = AS_InstrList(AS_Label(fun_label, frame->name), NULL);
    head_inst_ptr = head_inst_list;

    /** 函数入口处的汇编指令 */

    int recover_offset;
    if (frame->isLeaf) {
        char *inst = (char *) "\tstr     fp, [sp, #-4]!\n";
        head_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
        head_inst_ptr = head_inst_ptr->tail;

        recover_offset = 0;
    } else {
        char *inst = (char *) "\tstmfd   sp!, {fp, lr}\n";
        head_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
        head_inst_ptr = head_inst_ptr->tail;

        recover_offset = 1 * word_size;
    }

    // todo callee 保护的寄存器的相关指令

    char *inst = (char *) checked_malloc(sizeof(char) * 20);
    sprintf(inst, "\tadd     fp, sp, #%d\n", recover_offset);
    head_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
    head_inst_ptr = head_inst_ptr->tail;

    int space = getSpace(frame);// todo 此处还应有要保护寄存器空间
    if (space>0){
        char *frame_space = (char*)checked_malloc(sizeof(char) * 20);
        sprintf(frame_space, "\tsub     sp, sp, #%d\n", space * word_size);
        head_inst_ptr->tail = AS_InstrList(AS_Oper(frame_space, NULL, NULL, NULL), NULL);
        head_inst_ptr = head_inst_ptr->tail;
    }

    /** 将函数入口指令和body指令连接 */
    head_inst_ptr->tail = body->tail;

    /** 函数出口指令 */
    inst = (char *) checked_malloc(sizeof(char) * 20);
    sprintf(inst, "\tsub     sp, fp, #%d\n", recover_offset);
    tail_inst_list = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
    tail_inst_ptr = tail_inst_list;

    if (frame->isLeaf) {
        inst = (char *) "\tldr     fp, [sp], #4\n";
        tail_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
        tail_inst_ptr = tail_inst_ptr->tail;
    } else {
        inst = (char *) "\tldmfd   sp!, {fp, lr}\n";
        tail_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);
        tail_inst_ptr = tail_inst_ptr->tail;
    }

    inst = (char *) checked_malloc(sizeof(char) * 20);
    sprintf(inst, "\tbx      lr\n");
    tail_inst_ptr->tail = AS_InstrList(AS_Oper(inst, NULL, NULL, NULL), NULL);

    /** 连接上出口指令 */
    AS_instrList total_inst = AS_splice(head_inst_list, tail_inst_list);

    char *entry_meta = (char *) checked_malloc(sizeof(char) * 100);
    sprintf(entry_meta, "\t.align  2\n\t.global %s\n\t.type   %s, %%function\n", name, name);
    char *exit_meta = (char *) checked_malloc(sizeof(char) * 100);
    sprintf(exit_meta, "\t.size   %s, .-%s", name, name);

    return AS_Proc(entry_meta, total_inst, exit_meta);
}


Temp_map F_tempMap = nullptr;
