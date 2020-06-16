//
// Created by loyx on 2020/5/7.
//

#include "translate.h"



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
struct Tr_access_{
    Tr_level level;
    F_access access;
};//为静态链变量回溯 添加level封装
struct Tr_expList_{ /// 特殊list, 记录头和尾
    Tr_node first;
    Tr_node last;
};
struct Tr_node_{
    Tr_exp exp;
    Tr_node next;
};
struct Tr_level_{
    Tr_level_ parent_level;
    F_frame frame;
};//回溯用栈帧level封装

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
 Tr_access Tr_Access(Tr_level level,F_access access)//接口改变
 {
    Tr_access acc=(Tr_access)checked_malloc(sizeof(*acc));
    acc->level=level;
    acc->access=access;
    return acc;
 }
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail){
    Tr_accessList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

static Tr_exp Tr_Ex(T_exp exp){
    Tr_exp ex = checked_malloc(sizeof(*ex));
    ex->kind = Tr_ex;
    ex->u.ex = exp;
    return ex;
}
static Tr_exp Tr_Nx(T_stm stm){
    Tr_exp nx = checked_malloc(sizeof(*nx));
    nx->kind = Tr_nx;
    nx->u.nx = stm;
    return nx;
}
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm){
    Tr_exp cx = checked_malloc(sizeof(*cx));
    cx->kind = Tr_cx;
    cx->u.cx.trues = trues;
    cx->u.cx.falses = falses;
    cx->u.cx.stm = stm;
    return cx;
}

static patchList PatchList(Temp_label *head, patchList tail){
    patchList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}


/** delegate to frame.h */
Tr_access Tr_allocLocal(Tr_level level, bool escape){
    F_access new_access=F_allocLocal(level->frame, escape);
    return Tr_Access(level,new_access);
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
Tr_level Tr_newLevel(Tr_level parent,Temp_label name,U_boolList formals)//
{
    Tr_level level=(Tr_level)checked_malloc(sizeof(Tr_level_));
    level->parent=parent;
    level->frame=F_newFrame(name,U_boolList(true,formals));
    return level;
}//level封装后的newframe
Tr_expList Tr_ExpList(){
    Tr_expList p = checked_malloc(sizeof(*p));
    p->last = NULL;
    p->first = NULL;
    return p;
}
void Tr_expList_append(Tr_expList list, Tr_exp exp){
    if (list->first){
        Tr_node node = checked_malloc(sizeof(*node));
        node->exp = exp;
        node->next = NULL;
        list->last->next = node;
        list->last = list->last->next;
    } else{
        Tr_expList_prepend(list, exp);
    }
}
void Tr_expList_prepend(Tr_expList list, Tr_exp exp){
    if (list->first){
        Tr_node node = checked_malloc(sizeof(*node));
        node->exp = exp;
        node->next = list->first;
        list->first = node;
    } else{
        list->first = checked_malloc(sizeof(*list->first));
        list->first->exp = exp;
        list->first->next = NULL;
        list->last = list->first;
    }
}
bool Tr_expList_isEmpty(Tr_expList list){
    if (!list || !list->first) return true;
    return false;
}
Tr_exp Tr_simpleVar(Tr_access acc,Tr_level level)//acc中包含目标access的offset和层数，level是指当前层数（静态连访问从当前层数逐层回溯访问）
{
    T_exp tmp=T_Temp(F_FP());
    while(level!=NULL&&level!=acc->level->parent)
    {
        tmp=T_Mem(T_Binop(T_add,T_Const(level->frame->formals->head->u.offset),tmp));
        level=level->parent;
    }
    return Tr_Ex(F_Exp(acc->access,tmp));
}
//todo array trans
