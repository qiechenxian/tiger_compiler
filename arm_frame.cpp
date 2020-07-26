//
// Created by loyx on 2020/5/10.
//
#include "frame.h"
#include "assem.h"
const int F_WORD_SIZE = 4; /// 32位机器
static const int F_K = 4; /// 保存在Reg中参数的数量(待定)
static Temp_temp fp = nullptr;
static Temp_temp sp = nullptr;
static Temp_temp zero = nullptr;
static Temp_temp ra = nullptr;
static Temp_temp rv = nullptr;

//栈帧结构
Temp_temp F_FP()//取帧指针
{
    if(fp==nullptr)
    {
        char save[5];
        sprintf(save, "%s", "FP");
        fp=Temp_new_special(save);
    }
    return fp;
}
Temp_temp F_RV()//取帧指针
{
    if(rv==nullptr)
    {
        char save[5];
        sprintf(save, "%s", "RV");
        rv=Temp_new_special(save);
    }
    return rv;
}
Temp_temp F_SP(void) {
    if (sp == nullptr) {
        sp = Temp_newTemp();
    }
    return sp;
}

Temp_temp F_ZERO(void) {
    if (zero == nullptr) {
        zero = Temp_newTemp();
    }
    return zero;
}

Temp_temp F_RA(void) {
    if (ra == nullptr) {
        ra = Temp_newTemp();
    }
    return ra;
}
/** class declare */
struct F_access_{
    enum {inFrame, inReg, inGlobal}kind;
    struct {
        int offset;
        Temp_temp reg;
        Temp_label global;
    }u;
};

struct F_frame_{
    Temp_label name;
    F_accessList formals;
    F_accessList locals;
    int local_count;
    /** instructions required view shift*/
};//添加局部变量域



/** function prototype */
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);
static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals);


/** 构造函数 */
static F_access InFrame(int offset){
    F_access fa = (F_access)checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inFrame;
    fa->u.offset = offset;
    return fa;
}
static F_access InReg(Temp_temp reg){
    F_access fa = (F_access)checked_malloc(sizeof(*fa));
    fa->kind = F_access_::inReg;
    fa->u.reg = reg;
    return fa;
}
static F_access InGlobal(Temp_label global){
    auto fa = (F_access)checked_malloc(sizeof(F_access_));
    fa->kind = F_access_::inGlobal;
    fa->u.global = global;
    return fa;
}
static F_accessList F_AccessList(F_access head, F_accessList tail){
    F_accessList p = (F_accessList)checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

Temp_label F_getGlobalLabel(F_access fa){
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
 *      旧SP->  |       a4      |
 *        FP->  |   旧LR,FP等    |
 *              |   局部变量等    |
 *              |       a1      |
 *              |       a2      |
 *              |       a3      |
 *              |       a4      |
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
    for (U_boolList iter = formals; iter; iter = iter->tail){
        F_access access = nullptr;
        if (args_inReg_cnt <= F_K&&(iter->head==false)&&false){//暂时采取全部放在堆栈的存储方式,之后进行寄存器分配优化时修改
            access = InReg(Temp_newTemp());
            args_inReg_cnt++;
        } else{
            access = InFrame(args_inFrame_cnt++ * F_WORD_SIZE);
        }
        if (access_list){
            list_tail->tail = F_AccessList(access, nullptr);
            list_tail = list_tail->tail;
        } else{
            access_list = F_AccessList(access, nullptr);
            list_tail = access_list;
        }
    }
    return access_list;
}


/** frame 相关 */
F_frame F_newFrame(Temp_label name, U_boolList formals){
    F_frame f = (F_frame)checked_malloc(sizeof(*f));
    f->name = name;
    f->formals = makeFormalAccessList(f, formals);
    f->local_count = 1; ///为保存旧FP预留空间 todo 当该函数为子叶函数时，可优化掉栈帧 --loyx 2020/7/25
    f->locals=nullptr;
    return f;
}
Temp_label F_getName(F_frame frame){
    return frame->name;
}
F_accessList F_getFormals(F_frame frame){
    return frame->formals;
}
F_access F_allocLocal(F_frame frame, bool escape, int size){
    frame->local_count += size;
    if (escape) {
        F_access access=InFrame(F_WORD_SIZE  * (- frame->local_count));
        frame->locals=F_AccessList(access,frame->locals);
        return access;
    }
    else
    {
        return InReg(Temp_newTemp());
    }
}
F_access F_allocGlobal(S_symbol global){
    /**
     * 仅返回一个全局变量的label
     */
    return InGlobal((Temp_label)global);
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
    return T_Mem(T_Binop(T_add, framePtr, T_Const(get_word_size()*index + acc->u.offset)));
}

int get_word_size()
{
    return F_WORD_SIZE;
}
F_accessList F_formals(F_frame f) {
    return f->formals;
}

/// 片段相关F_frag
F_fragList F_FragList(F_frag head,F_fragList tail)
{
    F_fragList new_frag_list=(F_fragList)checked_malloc(sizeof(*new_frag_list));
    new_frag_list->head=head;
    new_frag_list->tail=tail;
    return new_frag_list;
}
F_frag F_StringFrag(Temp_label label,c_string str)
{
    F_frag new_frag=(F_frag)checked_malloc(sizeof(*new_frag));
    new_frag->kind=F_frag_::F_stringFrag;
    new_frag->u.stringg.str=str;
    new_frag->u.stringg.label=label;
    return new_frag;
}
F_frag F_ProcFrag(T_stm body,F_frame frame)
{
    F_frag new_frag=(F_frag)checked_malloc(sizeof(*new_frag));
    new_frag->kind=F_frag_::F_procFrag;
    new_frag->u.proc.body=body;
    new_frag->u.proc.frame=frame;
    return new_frag;
}
F_frag F_GlobalFrag(Temp_label label, int size, U_pairList init_values){
    auto new_frag = (F_frag)checked_malloc(sizeof(F_frag_));
    new_frag->kind = F_frag_::F_globalFrag;
    new_frag->u.global.label = label;
    new_frag->u.global.size = size;
    new_frag->u.global.init_values = init_values;
    return new_frag;
}
T_stm F_procEntryExit1(F_frame frame,T_stm stm)
{
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
                                   Temp_TempList(F_RA(),
                                                 Temp_TempList(F_SP(), calleeSaves)));
    return AS_splice(body, AS_InstrList(
            AS_Oper("", nullptr, returnSink, nullptr), nullptr));
}

/*
 *
 */
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
    char buf[100];
    sprintf(buf, "PROCEDURE %s\n", S_name(frame->name));
    return AS_Proc(String(buf), body, "END\n");
}


Temp_map F_tempMap = nullptr;