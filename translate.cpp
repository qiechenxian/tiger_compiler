//
// Created by loyx on 2020/5/7.
//

#include "translate.h"
#include "tree.h"


/**
 * typedefs
 * */
typedef struct Tr_node_* Tr_node;



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


/**
 * function prototype
 */
static Tr_exp Tr_Ex(T_exp exp);
static Tr_exp Tr_Nx(T_stm stm);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
static T_exp Tr_unEx(Tr_exp exp);
static T_stm Tr_unNx(Tr_exp exp);
static struct Cx unCx(Tr_exp exp);

static patchList PatchList(Temp_label *head, patchList tail);
static void doPatch(patchList list, Temp_label label);
static patchList joinPatch(patchList fList, patchList sList);


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
static Tr_exp Tr_Nx(T_stm stm){
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
            T_stm stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL ,  NULL);
            struct Cx cx;
            cx.stm = stm;
            cx.falses = PatchList(&stm->u.CJUMP.falses, NULL);
            cx.trues = PatchList(&stm->u.CJUMP.trues, NULL);
            return cx;
        }
        case Tr_exp_::Tr_nx:
        {
            printf("error:transform nx to cx");
        }
    }
}

static patchList PatchList(Temp_label *head, patchList tail){
    patchList p = (patchList)checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}


/** delegate to frame.h */
Tr_access Tr_allocLocal(F_frame frame, bool escape){
    return F_allocLocal(frame, escape);
}
Tr_accessList Tr_getFormals(Tr_frame frame){
    return F_getFormals(frame);
}
Tr_frame Tr_newFrame(Temp_label name, U_boolList formals){
    return F_newFrame(name, formals);
}


/** Tr_exp */


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
    p->last = NULL;
    p->first = NULL;
    return p;
}
//void Tr_expList_append(Tr_expList list, Tr_exp exp){
//    if (list->first){
//        Tr_node node = (Tr_node)checked_malloc(sizeof(*node));
//        node->exp = exp;
//        node->next = NULL;
//        list->last->next = node;
//        list->last = list->last->next;
//   } else{
//        Tr_expList_prepend(list, exp);
//    }
//}
//void Tr_expList_prepend(Tr_expList list, Tr_exp exp){
//    if (list->first){
//        Tr_node node = (Tr_node)checked_malloc(sizeof(*node));
//        node->exp = exp;
//        node->next = list->first;
//        list->first = node;
//    } else{
//        list->first = (Tr_node)checked_malloc(sizeof(*list->first));
//        list->first->exp = exp;
//        list->first->next = NULL;
//        list->last = list->first;
//    }
//}
void Tr_expList_append(Tr_expList list, Tr_exp exp) {
    if(list->first==NULL)
    {
        list->first=exp;
        return;
    }
    while(list->last!=NULL)
    {
        list=list->last;
    }
    Tr_expList p = (Tr_expList)checked_malloc(sizeof(*p));
    p->first=exp;
    p->last=NULL;
    list->last=p;
}
bool Tr_expList_isEmpty(Tr_expList list){
    if (!list || !list->first) return true;
    return false;
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
    printf("error:wrong with Tr_simpleVar from translate.c");
}

Tr_exp Tr_subsriptVar(Tr_exp base, Tr_exp offset) {
    return Tr_Ex(T_Mem(T_Binop(T_add,Tr_unEx(base),T_Binop(T_mul, Tr_unEx(offset), T_Const(get_word_size())))));
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
        default:printf("error from Tr_binop translate.c maybe something wrong wirh binop");break;
    }
    if(op==T_add&& left==NULL)
    {
        op=T_s_add;
    }
    if(op==T_sub&& left==NULL)
    {
        op=T_s_sub;
    }
    if(op==T_add||op==T_sub||op==T_mul||op==T_div||op==T_mod||op==T_and||op==T_or)
    {
        return Tr_Ex(T_Binop(op,Tr_unEx(left),Tr_unEx(right)));
    }
    else if(op==T_s_add||op==T_s_sub||op==T_not)
    {
        return Tr_Ex(T_Binop(op, nullptr,Tr_unEx(right)));
    }
    else
    {
        printf("error:wrong with Tr_binop from translate.c");
        return NULL;
    }
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
        default:printf("error from Tr_binop translate.c maybe something wrong wirh relop");break;
    }
    T_exp left_exp=Tr_unEx(left);
    T_exp right_exp=Tr_unEx(right);
}

Tr_exp Tr_if_else(Tr_exp condition_part,Tr_exp then_part,Tr_exp else_part)//if_else语句
{
    Temp_label if_t = Temp_newLabel();
    Temp_label if_f = Temp_newLabel();
    struct Cx cx_condi=Tr_unCx(condition_part);
    Temp_temp if_r=Temp_newTemp();
    doPatch(cx_condi.trues,if_t);
    doPatch(cx_condi.falses,if_f);
    T_stm then_stm=NULL;
    T_stm else_stm=NULL;

    switch (then_part->kind)
    {
        case Tr_exp_::Tr_cx:{printf("error:grammer may not allowed,  wrong with Tr_if_else from translate.c");break;}
        case Tr_exp_::Tr_nx:
        case Tr_exp_::Tr_ex:{then_stm= T_Seq(cx_condi.stm,T_Seq(T_Label(if_t),Tr_unNx(then_part))); break;}
    }
    if(else_part==NULL)
    {
        return(Tr_Nx(T_Seq(then_stm,T_Label(if_f))));
    }
    else if(else_part!=NULL)
    {
        Temp_label if_e=Temp_newLabel();
        then_stm=T_Seq(then_stm,T_Jump(T_Name(if_e),Temp_LabelList(if_e,NULL)));
        switch (else_part->kind)
        {
            case Tr_exp_::Tr_cx:{printf("error:grammer may not allowed,  wrong with Tr_if_else from translate.c");break;}
            case Tr_exp_::Tr_nx:
            case Tr_exp_::Tr_ex:{else_stm= T_Seq(then_stm,T_Seq(T_Label(if_f),Tr_unNx(else_part)));break;}
        }
        return(Tr_Nx(T_Seq(else_stm,T_Label(if_e))));
    }
}

Tr_exp Tr_dec_Var(Tr_expList exps,int size)//变量或数组声明无初值
{
    Temp_temp dec_var_r=Temp_newTemp();
    T_exp pointer=T_Call(T_Name(Temp_namedLabel("init_var")),T_ExpList(T_Const(size*get_word_size()),NULL));//调用外部函数，应该是汇编实现
    T_stm dec_var=T_Move(T_Temp(dec_var_r),pointer);
    return Tr_Ex(T_Eseq(dec_var,T_Temp(dec_var_r)));
}

Tr_exp Tr_init_Var(Tr_expList exps,Tr_expList sizes)//变量或数组声明带有初值sizes为数组长度信息 exps为
{
    T_expList call_size=T_ExpList(Tr_unEx(sizes->first), nullptr);
    sizes=sizes->last;
    T_expList v_temp=call_size;
    Temp_temp dec_var_r=Temp_newTemp();
    for(;sizes;sizes=sizes->last)
    {
        v_temp->tail=T_ExpList(Tr_unEx(sizes->first), nullptr);
        v_temp=v_temp->tail;
    }
    T_exp pointer=T_Call(T_Name(Temp_namedLabel("init_var")),call_size);//调用外部函数，应该是汇编实现
    T_stm init_var=T_Move(T_Temp(dec_var_r),pointer);
    for(int i=0;exps;exps=exps->last,i++)
    {
        init_var=T_Seq(init_var,T_Move(T_Mem(T_Binop(T_add,T_Const(i*get_word_size()),T_Temp(dec_var_r))),Tr_unEx(exps->first)));
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
    return Tr_Nx(T_Jump(T_Name(done_l),Temp_LabelList(done_l,NULL)));
}
Tr_exp Tr_continue(Tr_exp initial)
{
    Temp_label initial_l=Tr_unEx(initial)->u.NAME;
    return Tr_Nx(T_Jump(T_Name(initial_l),Temp_LabelList(initial_l,NULL)));
}
Tr_exp Tr_while(Tr_exp w_cond,Tr_exp w_stmt,Tr_exp w_break,Tr_exp w_continue)
{
    Temp_label while_f=NULL;
    Temp_label while_i=NULL;
    if(w_break==NULL)
    {
        while_f=Temp_newLabel();
    } else
    {
        while_f=Tr_unEx(w_break)->u.NAME;
    }
    if(w_continue==NULL)
    {
        while_i=Temp_newLabel();
    } else
    {
        while_i=Tr_unEx(w_continue)->u.NAME;
    }
    struct Cx cx_condi=Tr_unCx(w_cond);
    T_stm body_stm=NULL;
    switch (w_stmt->kind)
    {
        case Tr_exp_::Tr_cx:{printf("error:grammer may not allowed,  wrong with Tr_if_else from translate.c");break;}
        case Tr_exp_::Tr_nx:
        case Tr_exp_::Tr_ex:{body_stm=Tr_unNx(w_stmt);body_stm=T_Seq(T_Label(while_i),T_Seq(cx_condi.stm,T_Seq(body_stm,T_Seq(T_Jump(T_Name(while_i),Temp_LabelList(while_i,NULL)),T_Label(while_f))))); break;}
    }
    return Tr_Nx(body_stm);
}
Tr_exp Tr_func_call(Temp_label name,Tr_expList params)
{
    T_exp tmp=T_Temp(F_FP());
    T_expList call_param_h=T_ExpList(tmp,NULL);
    T_expList c_temp=call_param_h;
    for(;params;params=params->last)
    {
        c_temp->tail=T_ExpList(Tr_unEx(params->first),NULL);
        c_temp=c_temp->tail;
    }
    return Tr_Ex(T_Call(T_Name(name),call_param_h));
}
Tr_exp Tr_assign(Tr_exp left,Tr_exp right)
{
    return Tr_Nx(T_Move(Tr_unEx(left),Tr_unEx(right)));
}
Tr_exp Tr_seq(Tr_exp left,Tr_exp right)
{
    return Tr_Nx(T_Seq(Tr_unNx(left),Tr_unNx(right)));
}
//todo return translate
