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

struct Tr_expList_{ /// 特殊list, 记录头和尾
    Tr_node first;
    Tr_node last;
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

