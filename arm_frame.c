//
// Created by loyx on 2020/5/10.
//
#include "frame.h"

/** 架构参数 */
const int F_WORD_SIZE = 4; /// 32位机器
static const int F_K = 6; /// 保存在Reg中参数的数量(待定)
static Temp_temp fp=NUll;
/** class declare */
struct F_access_{
    enum {inFrame, inReg}kind;
    struct {
        int offset;
        Temp_temp reg;
    }u;
};

struct F_frame_{
    Temp_label name;
    F_accessList formals;
    F_accessList locals;
    int local_count;
    /** instructions required view shift*/
};//添加局部变量域，formals域栈顶默认为静态链
Temp_temp F_Fp()//取帧指针
{
    if(fp==NULL)
    {
        fp=Temp_newTemp();
    }
    return fp;
}

/** function prototype */
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);
static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals);


/** 构造函数 */
static F_access InFrame(int offset){
    F_access fa = checked_malloc(sizeof(*fa));
    fa->kind = inFrame;
    fa->u.offset = offset;
    return fa;
}
static F_access InReg(Temp_temp reg){
    F_access fa = checked_malloc(sizeof(*fa));
    fa->kind = inReg;
    fa->u.reg = reg;
    return fa;
}
static F_accessList F_AccessList(F_access head, F_accessList tail){
    F_accessList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}


/** 辅助函数 */
static F_accessList makeFormalAccessList(F_frame frame, U_boolList formals){
    F_accessList access_list = NULL, list_tail = NULL;
    int args_inReg_cnt = 0;
    for (U_boolList iter = formals; iter; iter = iter->tail){
//        assert(iter->head); // todo escape
        F_access access = NULL;
        if (args_inReg_cnt <= F_K&&(iter->head==false)&&false){//暂时采取全部放在堆栈的存储方式,之后进行寄存器分配优化时修改
            access = InReg(Temp_newTemp());
            args_inReg_cnt++;
        } else{
            // todo 查看具体架构
            access = InFrame((1 + args_inReg_cnt) * F_WORD_SIZE);//
        }
        if (access_list){
            list_tail->tail = F_AccessList(access, NULL);
            list_tail = list_tail->tail;
        } else{
            access_list = F_AccessList(access, NULL);
            list_tail = access_list;
        }
    }
    return access_list;
}


/** frame 相关 */
F_frame F_newFrame(Temp_label name, U_boolList formals){
    F_frame f = checked_malloc(sizeof(*f));
    f->name = name;
    f->formals = makeFormalAccessList(f, formals);
    f->local_count = 0;
    f->locals=NULL;
    return f;
}
Temp_label F_getName(F_frame frame){
    return frame->name;
}
F_accessList F_getFormals(F_frame frame){
    return frame->formals;
}
F_access F_allocLocal(F_frame frame, bool escape){
    frame->local_count++;
    F_access access;
    if (escape) {
        access=InFrame(F_WORD_SIZE  * (- frame->local_count));
        frame->locals=F_AccessList(access,frame->locals);
    }
    else
    {
        return InReg(Temp_newTemp());
    }
}
T_exp F_Exp(F_access acc, T_exp framePtr)//将F_access转换为tree表达式
{
    if (acc->kind ==inFrame )
    {
        return T_Mem(T_Binop(T_add, framePtr, T_Const(acc->u.offset)));
    }
    return  T_Temp(acc->u.reg);
}


