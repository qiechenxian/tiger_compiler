//
// Created by loyx on 2020/5/7.
//

#include "translate.h"


/**
 * typedefs
 * */
typedef struct Tr_node_* Tr_node;
static F_fragList fragList = nullptr;


/**
 * class declare
 * */
struct patchList_{
    Temp_label *head;
    patchList tail;
};
struct Cx{
    patchList trues;
    patchList falses;
    T_stm stm;
};
struct Tr_exp_{
    enum {Tr_ex, Tr_nx, Tr_cx}kind;
    union {
        T_exp ex;
        T_stm nx;
        struct Cx cx;
    }u;
};

//struct Tr_expList_{ /// 特殊list, 记录头和尾
//    Tr_node first;
//    Tr_node last;
//};
struct Tr_expList_{ /// 特殊list, 记录头和尾
    Tr_exp first;
    Tr_expList last;
};
struct Tr_node_{
    Tr_exp exp;
    Tr_node next;
};
static Tr_frame root_frame = nullptr;

/**
 * function prototype
 */
static Tr_exp Tr_Ex(T_exp exp);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
static T_exp Tr_unEx(Tr_exp exp);
static T_stm Tr_unNx(Tr_exp exp);
static struct Cx unCx(Tr_exp exp);

static patchList PatchList(Temp_label *head, patchList tail);
static void doPatch(patchList list, Temp_label label);
static patchList joinPatch(patchList fList, patchList sList);

Tr_frame Tr_root_frame(void)
{
    return nullptr;
}
/**
 * 构造函数
 * */
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail){
    Tr_accessList p = (Tr_accessList)checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

static Tr_exp Tr_Ex(T_exp exp){
    Tr_exp ex = (Tr_exp)checked_malloc(sizeof(*ex));
    ex->kind = Tr_exp_::Tr_ex;
    ex->u.ex = exp;
    return ex;
}
Tr_exp Tr_Nx(T_stm stm){
    Tr_exp nx = (Tr_exp)checked_malloc(sizeof(*nx));
    nx->kind = Tr_exp_::Tr_nx;
    nx->u.nx = stm;
    return nx;
}
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm){
    Tr_exp cx = (Tr_exp)checked_malloc(sizeof(*cx));
    cx->kind = Tr_exp_::Tr_cx;
    cx->u.cx.trues = trues;
    cx->u.cx.falses = falses;
    cx->u.cx.stm = stm;
    return cx;
}
static T_exp Tr_unEx(Tr_exp e)
{
    switch(e->kind)
    {
        case Tr_exp_::Tr_ex:
            return e->u.ex ;
        case Tr_exp_::Tr_cx :
        {
            Temp_temp tmp =Temp_newTemp();
            Temp_label t =Temp_newLabel() , f = Temp_newLabel() ;
            doPatch(e->u.cx.falses , f) ;
            doPatch(e->u.cx.trues , t) ;
            return T_Eseq(T_Move(T_Temp(tmp) , T_Const(1)),
                          T_Eseq( e->u.cx.stm ,
                                  T_Eseq( T_Label(f) ,
                                          T_Eseq(T_Move(T_Temp(tmp) , T_Const(0)) ,
                                                 T_Eseq(T_Label(t) ,
                                                        T_Temp(tmp)))))) ;

        }
        case Tr_exp_::Tr_nx:
        {
            return T_Eseq(e->u.nx, T_Const(0));
        }
    }
}
static T_stm Tr_unNx(Tr_exp e)
{
    switch (e->kind)
    {
        case  Tr_exp_::Tr_ex:
            return T_Exp(e->u.ex);
        case  Tr_exp_::Tr_cx:
            return T_Exp(Tr_unEx(e));
        case  Tr_exp_::Tr_nx:
            return e->u.nx;
    }
}
static struct Cx Tr_unCx(Tr_exp e)
{
    switch (e->kind)
    {
        case Tr_exp_::Tr_cx :
            return e->u.cx;
        case Tr_exp_::Tr_ex:
        {
            T_stm stm = T_Cjump(T_ne, e->u.ex, T_Const(0), nullptr ,  nullptr);
            struct Cx cx{};
            cx.stm = stm;
            cx.falses = PatchList(&stm->u.CJUMP.falses, nullptr);
            cx.trues = PatchList(&stm->u.CJUMP.trues, nullptr);
            return cx;
        }
        case Tr_exp_::Tr_nx:
        {
            printf("error:transform nx to cx");
            assert(0);
        }
    }
}

static patchList PatchList(Temp_label *head, patchList tail){
    patchList p = (patchList)checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}
static patchList connect_PatchList(patchList left, patchList right){
    patchList head=PatchList(nullptr, nullptr);
    patchList temp=head;
    for(;left;left=left->tail)
    {
        if(temp->head== nullptr)
        {
            temp->head=left->head;
        } else
        {
            temp->tail=PatchList(left->head, nullptr);
            temp=temp->tail;
        }
    }
    for(;right;right=right->tail)
    {
        if(temp->head== nullptr)
        {
            temp->head=right->head;
        } else
        {
            temp->tail=PatchList(right->head, nullptr);
            temp=temp->tail;
        }
    }
    return head;
}

/** delegate to frame.h */
Tr_access Tr_allocLocal(F_frame frame, bool escape, int size){
    return F_allocLocal(frame, escape, size);
}
Tr_access Tr_allocGlobal(S_symbol global){
    return F_allocGlobal(global);
}
Tr_accessList Tr_getFormals(Tr_frame frame){
    return F_getFormals(frame);
}
Tr_frame Tr_newFrame(Temp_label name, U_boolList formals){
    return F_newFrame(name, formals);
}
Temp_label Tr_getGlobalLabel(Tr_access access){
    /**
     * 从Tr_access中获取全局变量的label，该函数解耦了semant和frame
     */
    return F_getGlobalLabel(access);
}


/** 全局变量的frag */
void Tr_newIntFrag(Temp_label label, int value, bool comm){
    U_pairList pair_list = U_PairList(U_IntPair(1, value), nullptr);
    F_frag int_frag = F_GlobalFrag(label, 1, pair_list, comm);
    fragList = F_FragList(int_frag, fragList);
}
void Tr_newArrayFrag(Temp_label label, int size, U_pairList values, bool comm){
    F_frag array_frag = F_GlobalFrag(label, size, values, comm);
    fragList = F_FragList(array_frag, fragList);
}

/** patchList 相关*/
static void doPatch(patchList list, Temp_label label){
    for (; list; list = list->tail){
        *(list->head) = label;
    }
}
static patchList joinPatch(patchList fList, patchList sList){
    if (!fList) return sList;
    patchList joinList = fList;
    for (; fList->tail; fList = fList->tail);
    fList->tail = sList;
    return joinList;
}


/** expList functions */
Tr_expList Tr_ExpList(){
    Tr_expList p = (Tr_expList)checked_malloc(sizeof(*p));
    p->last = nullptr;
    p->first = nullptr;
    return p;
}
void Tr_expList_append(Tr_expList list, Tr_exp exp) {
    if(list->first==nullptr)
    {
        list->first=exp;
        return;
    }
    while(list->last!=nullptr)
    {
        list=list->last;
    }
    Tr_expList p = (Tr_expList)checked_malloc(sizeof(*p));
    p->first=exp;
    p->last=nullptr;
    list->last=p;
}
bool Tr_expList_isEmpty(Tr_expList list){
    return !list || !list->first;
}
Tr_exp Tr_nopExp()
{
    return Tr_Ex(T_Const(0));
}
Tr_exp Tr_intExp(int int_value)
{
    return Tr_Ex(T_Const(int_value));
}



//以下为转换为中间代码
Tr_exp Tr_simpleVar(Tr_access acc)
{
    Temp_temp get_fp=F_FP();
    T_exp tmp=T_Temp(get_fp);
    return Tr_Ex(F_Exp(acc,tmp));
}

Tr_exp Tr_simpleVarNoMem(Tr_access acc)
/**
 * 在访问数组且未定位到元素时(例如：int a[2][4][3]; a[1][1] 或 a)，翻译结果为地址(不带T_MEM)
 * 该函数是为了翻译不带下标的数组(例如上一行中的 a )，使其结果仅为地址计算
 * 同时为 a[1][1] 这种访问的正确翻译提供基础
 * @param acc  变量的acc (！注意：仅为数组变量服务)
 * @return 不为T_MEM的翻译
 */
{
    T_exp acc_with_mem = F_Exp(acc, T_Temp(F_FP()));
    assert(acc_with_mem->kind == T_exp_::T_MEM);
    return Tr_Ex(acc_with_mem->u.MEM);
}

Tr_exp Tr_subscriptVar(Tr_exp base, Tr_exp offset, int dimension)
{
    return Tr_Ex(T_Mem(
            T_Binop(T_add,Tr_unEx(base),
                    T_Binop(T_mul, Tr_unEx(offset), T_Const(get_word_size()*dimension))
                    )));
}

Tr_exp Tr_subscriptVarNoMem(Tr_exp base, Tr_exp offset, int dimension)
{
    return Tr_Ex(T_Binop(
            T_add,
            Tr_unEx(base),
            T_Binop(T_mul, Tr_unEx(offset), T_Const(get_word_size()*dimension))
            ));
}

Tr_exp Tr_binop(A_binOp aop,Tr_exp left,Tr_exp right)//算术运算
{
    T_binOp op;
    switch (aop) {
        case A_add: op=T_add;break;
        case A_sub: op=T_sub;break;
        case A_mul: op=T_mul;break;
        case A_div: op=T_div;break;
        case A_mod: op=T_mod;break;
        default: {
            printf("error from Tr_binop translate.c maybe something wrong wirh binop");
            assert(0);
        }
    }
    if(op==T_add||op==T_sub||op==T_mul||op==T_div||op==T_mod)
    {
        return Tr_Ex(T_Binop(op,Tr_unEx(left),Tr_unEx(right)));
    }
    return Tr_Ex(T_Binop(op, Tr_unEx(left), Tr_unEx(right))); /// 简化了逻辑
}


Tr_exp Tr_relop(A_binOp aop,Tr_exp left,Tr_exp right)//逻辑运算
{
    T_relOp op;
    switch (aop) {
        case A_lt: op=T_lt;break;
        case A_le: op=T_le;break;
        case A_gt: op=T_gt;break;
        case A_ge: op=T_ge;break;
        case A_eq: op=T_eq;break;
        case A_ne: op=T_ne;break;
        case A_and:{
            op=T_and;
                if(left->kind==Tr_exp_::Tr_ex)
                {
                    T_relOp temp_op=T_gt;
                    T_exp left_exp=Tr_unEx(left);
                    T_exp right_exp=T_Const(0);
                    T_stm cond = T_Cjump(temp_op, left_exp, right_exp, nullptr, nullptr);
                    patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
                    patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
                    left->kind=Tr_exp_::Tr_cx;
                    left->u.cx.trues=trues;
                    left->u.cx.falses=falses;
                    left->u.cx.stm=cond;
                }
                if(right->kind==Tr_exp_::Tr_ex)
                {
                    T_relOp temp_op=T_gt;
                    T_exp left_exp=Tr_unEx(right);
                    T_exp right_exp=T_Const(0);
                    T_stm cond = T_Cjump(temp_op, left_exp, right_exp, nullptr, nullptr);
                    patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
                    patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
                    right->kind=Tr_exp_::Tr_cx;
                    right->u.cx.trues=trues;
                    right->u.cx.falses=falses;
                    right->u.cx.stm=cond;
                }
                Temp_label trues_and= Temp_newLabel();
                T_stm temp_stm=T_Seq(left->u.cx.stm,T_Seq(T_Label(trues_and),right->u.cx.stm));
                if(left->kind==Tr_exp_::Tr_cx&&right->kind==Tr_exp_::Tr_cx)
                {
                    doPatch(left->u.cx.trues,trues_and);
                    patchList and_trues= right->u.cx.trues;
                    patchList and_falses= connect_PatchList(left->u.cx.falses,right->u.cx.falses);
                    return Tr_Cx(and_trues,and_falses,temp_stm);
                }

            break;}
        case A_or:{
            op=T_or;
            if(left->kind==Tr_exp_::Tr_ex)//将A_exp装换为 A_exp>0   例如： （3||4）-->(3>0||4>0)
            {
                T_relOp temp_op=T_gt;
                T_exp left_exp=Tr_unEx(left);
                T_exp right_exp=T_Const(0);
                T_stm cond = T_Cjump(temp_op, left_exp, right_exp, nullptr, nullptr);
                patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
                patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
                left->kind=Tr_exp_::Tr_cx;
                left->u.cx.trues=trues;
                left->u.cx.falses=falses;
                left->u.cx.stm=cond;
            }
            if(right->kind==Tr_exp_::Tr_ex)//
            {
                T_relOp temp_op=T_gt;
                T_exp left_exp=Tr_unEx(right);
                T_exp right_exp=T_Const(0);
                T_stm cond = T_Cjump(temp_op, left_exp, right_exp, nullptr, nullptr);
                patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
                patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
                right->kind=Tr_exp_::Tr_cx;
                right->u.cx.trues=trues;
                right->u.cx.falses=falses;
                right->u.cx.stm=cond;
            }
            Temp_label falses_or= Temp_newLabel();
            T_stm temp_stm=T_Seq(left->u.cx.stm,T_Seq(T_Label(falses_or),right->u.cx.stm));
            if(left->kind==Tr_exp_::Tr_cx&&right->kind==Tr_exp_::Tr_cx)
            {
                doPatch(left->u.cx.falses,falses_or);
                patchList or_trues= connect_PatchList(left->u.cx.trues,right->u.cx.trues);
                patchList or_falses= right->u.cx.falses;
                return Tr_Cx(or_trues,or_falses,temp_stm);
            }
            break;}
        case A_not:
                {op=T_not;
                if(right->kind==Tr_exp_::Tr_cx)//条件语句交换true false label
                {
                    patchList temp=right->u.cx.falses;
                    right->u.cx.falses=right->u.cx.trues;
                    right->u.cx.trues=temp;
                } else if(right->kind==Tr_exp_::Tr_ex)//!3=!(3>0)=3<=0
                {
                    T_relOp temp_op=T_le;
                    T_exp left_exp=Tr_unEx(right);
                    T_exp right_exp=T_Const(0);
                    T_stm cond = T_Cjump(temp_op, left_exp, right_exp, nullptr, nullptr);
                    patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
                    patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
                    return Tr_Cx(trues,falses,cond);
                }
                break;}
        default:
            printf("error from Tr_binop translate.c maybe something wrong wirh relop");
            assert(0);
    }
    T_exp left_exp=Tr_unEx(left);
    T_exp right_exp=Tr_unEx(right);
    T_stm cond = T_Cjump(op, left_exp, right_exp, nullptr, nullptr);
    patchList trues = PatchList(&cond->u.CJUMP.trues, nullptr);
    patchList falses = PatchList(&cond->u.CJUMP.falses, nullptr);
    return Tr_Cx(trues,falses,cond);
}

Tr_exp Tr_if_else(Tr_exp condition_part,Tr_exp then_part,Tr_exp else_part)//if_else语句
{
    Temp_label if_t = Temp_newLabel();
    Temp_label if_f = Temp_newLabel();
    struct Cx cx_condi=Tr_unCx(condition_part);
    Temp_temp if_r=Temp_newTemp();
    doPatch(cx_condi.trues,if_t);
    doPatch(cx_condi.falses,if_f);
    T_stm then_stm=nullptr;
    T_stm else_stm=nullptr;

    switch (then_part->kind)
    {
        case Tr_exp_::Tr_cx:{
            printf("error:grammer may not allowed,"
                   "  wrong with Tr_if_else from translate.c");
            break;
        }
        case Tr_exp_::Tr_nx:
        case Tr_exp_::Tr_ex:{
            then_stm= T_Seq(cx_condi.stm,T_Seq(T_Label(if_t),Tr_unNx(then_part)));
            break;
        }
    }
    if(else_part==nullptr)
    {
        return(Tr_Nx(T_Seq(then_stm,T_Label(if_f))));
    }
    else {
        Temp_label if_e = Temp_newLabel();
        then_stm = T_Seq(then_stm,
                         T_Jump(T_Name(if_e),
                                Temp_LabelList(if_e, nullptr)));
        switch (else_part->kind) {
            case Tr_exp_::Tr_cx: {
                printf("error:grammer may not allowed,  wrong with Tr_if_else from translate.c");
                break;
            }
            case Tr_exp_::Tr_nx:
            case Tr_exp_::Tr_ex: {
                else_stm = T_Seq(then_stm, T_Seq(T_Label(if_f), Tr_unNx(else_part)));
                break;
            }
        }
        return (Tr_Nx(T_Seq(else_stm, T_Label(if_e))));
    }
}

Tr_exp Tr_init_array(Tr_access base, Tr_INIT_initList init_info)
/**
 * 栈内数组初始化翻译
 * @param base 栈内数组的基地址(access)
 * @param init_info 已翻译的初值
 * @return T_Seq
 */
{
    T_exp frame_ptr = T_Temp(F_FP());
    int array_length=init_info->array_length;
    T_stm init_var = T_Move(F_expWithIndex(base, frame_ptr, 0), Tr_unEx(init_info->array[0]));

    for(int i=1;i<array_length;i++)
    {
        init_var = T_Seq(
                init_var,
                T_Move(F_expWithIndex(base, frame_ptr, i), Tr_unEx(init_info->array[i]))
                );
    }
    return Tr_Nx(init_var);
}
Tr_exp Tr_doneExp() {
    return Tr_Ex(T_Name(Temp_newLabel()));
}
Tr_exp Tr_initialExp() {
    return Tr_Ex(T_Name(Temp_newLabel()));
}
Tr_exp Tr_break(Tr_exp done)
{
    Temp_label done_l=Tr_unEx(done)->u.NAME;
    return Tr_Nx(T_Jump(T_Name(done_l),Temp_LabelList(done_l,nullptr)));
}
Tr_exp Tr_continue(Tr_exp initial)
{
    Temp_label initial_l=Tr_unEx(initial)->u.NAME;
    return Tr_Nx(T_Jump(T_Name(initial_l),Temp_LabelList(initial_l,nullptr)));
}
Tr_exp Tr_while(Tr_exp w_cond,Tr_exp w_stmt,Tr_exp w_break,Tr_exp w_continue)
{
    Temp_label while_t=nullptr;
    Temp_label while_f=nullptr;
    Temp_label while_i=nullptr;
    if(w_break==nullptr)
    {
        while_f=Temp_newLabel();
    } else
    {
        while_f=Tr_unEx(w_break)->u.NAME;
    }
    if(w_continue==nullptr)
    {
        while_i=Temp_newLabel();
    } else
    {
        while_i=Tr_unEx(w_continue)->u.NAME;
    }
    while_t=Temp_newLabel();

    struct Cx cx_condi=Tr_unCx(w_cond);
    doPatch(cx_condi.trues,while_t);
    doPatch(cx_condi.falses,while_f);
    T_stm body_stm=nullptr;
    switch (w_stmt->kind)
    {
        case Tr_exp_::Tr_cx:{
            printf("error:grammer may not allowed,  wrong with Tr_if_else from translate.c");
            break;
        }
        case Tr_exp_::Tr_nx:
        case Tr_exp_::Tr_ex:{
            body_stm=Tr_unNx(w_stmt);
            body_stm=T_Seq(T_Label(while_i),
                    T_Seq(cx_condi.stm,
                            T_Seq(T_Label(while_t),T_Seq(body_stm,
                                         T_Seq(T_Jump(T_Name(while_i),
                                                      Temp_LabelList(while_i,nullptr)),
                                               T_Label(while_f)))) ));
            break;
        }
    }
    return Tr_Nx(body_stm);
}
Tr_exp Tr_func_call(Temp_label name,Tr_expList params)
{
    T_expList call_param_h=T_ExpList(nullptr,nullptr);
    T_expList c_temp= call_param_h;
    if(params->first== nullptr)
    {
        return Tr_Ex(T_Call(T_Name(name), nullptr));
    }
    for(;params;params=params->last)
    {
        if(call_param_h->head== nullptr)
        {
            c_temp=call_param_h;
            c_temp->head=Tr_unEx(params->first);
            continue;
        }
        c_temp->tail=T_ExpList(Tr_unEx(params->first),nullptr);
        c_temp=c_temp->tail;
    }
    return Tr_Ex(T_Call(T_Name(name),call_param_h));
}
Tr_exp Tr_assign(Tr_exp left,Tr_exp right)
{
    return Tr_Nx(T_Move(Tr_unEx(left),
            Tr_unEx(right)));
}
Tr_exp Tr_seq(Tr_exp left,Tr_exp right)
{
    return Tr_Nx(T_Seq(Tr_unNx(left),Tr_unNx(right)));
}
void Tr_procEntryExit(Tr_frame frame, Tr_exp body, Tr_accessList formals)//todo
{
    T_stm stm = Tr_unNx(body);
    stm = F_procEntryExit1(frame, stm);
    F_frag pfrag = F_ProcFrag(stm,frame);
    fragList = F_FragList(pfrag, fragList);
}
Tr_exp Tr_return(Tr_exp ret_num)
{
    Temp_temp get_rv=F_RV();
    T_exp tmp=T_Temp(get_rv);
    return Tr_Nx(T_Move(tmp,Tr_unEx(ret_num)));
}
//todo return jump label in chapter 12
Tr_exp Tr_newlabel()
{
    Temp_label new_l=Temp_newLabel();
    return Tr_Ex(T_Name(new_l));
}
F_fragList Tr_getResult(void)
{
    return (F_fragList)U_reverseList(fragList);
}
Tr_exp  Tr_add_fuc_head_label(Tr_exp returnValue,Temp_label fun_label)
{
     T_stm temp= Tr_unNx(returnValue);
     temp=T_Seq(T_Label(fun_label),temp);
     return Tr_Nx(temp);
}

Tr_exp Tr_StringExp(c_string content)
/**
 * 字符串字面量翻译，有两个操作：1.创建string frag并添加list 2.返回T_Name(label)
 * @param content 字符串
 * @return T_Name(Temp_label)
 */
{
    Temp_label str_pos = Temp_newStringLabel();
    F_frag str_frag = F_StringFrag(str_pos, content);
    fragList = F_FragList(str_frag, fragList);

    return Tr_Ex(T_Name(str_pos));
}
