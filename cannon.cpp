//
// Created by Administrator on 11/7/2020.
//
#include "cannon.h"
typedef struct expRefList_* expRefList;
struct expRefList_ {T_exp *head;expRefList tail;};
struct stmExp{T_stm s;T_exp e;};
static T_stm recoder(expRefList refList);
static T_stm do_stm(T_stm stm);
static struct stmExp do_exp(T_exp exp);
static expRefList get_call_reflist(T_exp callExp);
static T_stmList linear(T_stm stm,T_stmList right);
struct C_block global_block;
S_table block_env;
static T_stmList getLast(T_stmList list);
static void trace(T_stmList list);
static T_stmList getNext();

C_stmListList C_StmListList(T_stmList head,C_stmListList tail)
{
    C_stmListList block_list=(C_stmListList)checked_malloc(sizeof(*block_list));
    block_list->head=head;
    block_list->tail=tail;
    return block_list;
}
static bool isNop(T_stm x){
    return x->kind==T_stm_::T_EXP && x->u.EXP->kind==T_exp_::T_CONST;
}
static bool commute(T_stm x,T_exp y)
{
    return isNop(x)||y->kind==T_exp_::T_CONST||y->kind==T_exp_::T_NAME;
}
static T_stm seq(T_stm x,T_stm y){
    if(isNop(x))return y;
    if(isNop(y))return x;
    return T_Seq(x,y);
}
struct stmExp StmExp(T_stm s,T_exp e)
{
    struct stmExp new_stmExp{};
    new_stmExp.e=e;
    new_stmExp.s=s;
    return new_stmExp;
}
expRefList ExpRefList(T_exp *exp,expRefList tail)
{
    expRefList rfl=(expRefList)checked_malloc(sizeof(*rfl));
    rfl->head=exp;
    rfl->tail=tail;
    return rfl;
}
T_stmList C_linearize(T_stm stm)
{
    return linear(do_stm(stm),nullptr);
}
static T_stmList linear(T_stm stm,T_stmList right)
{
    if(stm->kind==T_stm_::T_SEQ)
    {
        return linear(stm->u.SEQ.left,linear(stm->u.SEQ.right,right));
    } else
    {return T_StmList(stm,right);}
}
static expRefList get_call_reflist(T_exp callExp) {
    expRefList res = ExpRefList(&callExp->u.CALL.fun, nullptr);
    expRefList p = res;
    T_expList p_args = callExp->u.CALL.args;
    while (p_args) {
        p->tail = ExpRefList(&p_args->head, nullptr);
        p_args = p_args->tail;
        p = p->tail;
    }
    return res;
}
static T_stm do_stm(T_stm stm) {
    switch (stm->kind) {
        case T_stm_::T_SEQ:
            return seq(do_stm(stm->u.SEQ.left), do_stm(stm->u.SEQ.right));
        case T_stm_::T_JUMP:
            return seq(recoder(ExpRefList(&stm->u.JUMP.exp, nullptr)),stm);
        case T_stm_::T_CJUMP:
            return seq(
                    recoder(
                            ExpRefList(&stm->u.CJUMP.left,
                                    ExpRefList(&stm->u.CJUMP.right, nullptr))),stm);
        case T_stm_::T_EXP:
            if (stm->u.EXP->kind == T_exp_::T_CALL)//call的处理
                return seq(recoder(get_call_reflist(stm->u.EXP)),stm);
            else
                return seq(recoder(ExpRefList(&stm->u.EXP, nullptr)),stm);
        case T_stm_::T_MOVE:
            if (stm->u.MOVE.dst->kind == T_exp_::T_TEMP) {
                if (stm->u.MOVE.src->kind == T_exp_::T_CALL)
                    return seq(recoder(get_call_reflist(stm->u.MOVE.src)),stm);
                else
                    return seq(recoder(
                            ExpRefList(&stm->u.MOVE.src, nullptr)),stm);
            } else if (stm->u.MOVE.dst->kind == T_exp_::T_MEM) {
                return seq(recoder(
                        ExpRefList(&stm->u.MOVE.dst->u.MEM,//注意这里，一般来说move的左端是目的操作数不会使用它的值，但move左端如果是Mem，其里面的寻址过程可能是有类似源操作数的操作，所以将&stm->u.MOVE.dst->u.MEM加入
                                ExpRefList(&stm->u.MOVE.src, nullptr))),stm);//例如MOVE(MEM(binop1),CALL(l0)),其中的binop1接可能被保存在寄存器
            } else if (stm->u.MOVE.dst->kind == T_exp_::T_ESEQ) {
                T_stm s = stm->u.MOVE.dst->u.ESEQ.stm;
                stm->u.MOVE.dst = s->u.MOVE.dst->u.ESEQ.exp;
                return do_stm(T_Seq(s, stm)); // todo:
            }
        default:
            return stm;
    }
}
static struct stmExp do_exp(T_exp exp){
    switch (exp->kind) {
        case T_exp_::T_BINOP:
            return StmExp(recoder(
                    ExpRefList(&exp->u.BINOP.left,
                            ExpRefList(&exp->u.BINOP.right, nullptr))),exp);
        case T_exp_::T_MEM:
            return StmExp(recoder(ExpRefList(&exp->u.MEM, nullptr)),exp);
        case T_exp_::T_ESEQ:{struct stmExp x=do_exp(exp->u.ESEQ.exp);
        return StmExp(seq(do_stm(exp->u.ESEQ.stm),x.s),x.e);
        }
        case T_exp_::T_CALL:
            return StmExp(recoder(get_call_reflist(exp)),exp);
        default:
            return StmExp(recoder(nullptr),exp);
    }
}
static T_stm recoder(expRefList refList) {
    if (!refList)
        return T_Exp(T_Const(0));
    if ((*refList->head)->kind == T_exp_::T_CALL) {
        Temp_temp temp = Temp_newTemp();
        // 对call的规则
        *refList->head = T_Eseq
                (T_Move(T_Temp(temp), *refList->head),T_Temp(temp));
    }
    struct stmExp head = do_exp(*refList->head);
    T_stm others = recoder(refList->tail);

    if (commute(others, head.e)) {
        *refList->head = head.e;
        return seq(head.s, others);
    } else
    {    Temp_temp temp = Temp_newTemp();
        *refList->head = T_Temp(temp);
        return seq(
                head.s,
                seq(T_Move(T_Temp(temp), head.e),others)
        );
    }
}
C_stmListList cut_stm(T_stmList pre_stm,T_stmList now_stm,Temp_label temp_done)//裁剪stm并完成开始新的block
{
    if(now_stm == nullptr)
    {
        T_stmList done_blcok= T_StmList(
                T_Jump(T_Name(temp_done),
                        Temp_LabelList(temp_done, nullptr)),
                        nullptr);
        pre_stm->tail=done_blcok;
        return nullptr;
    }
    else if(now_stm->head->kind==T_stm_::T_CJUMP||now_stm->head->kind==T_stm_::T_JUMP)
    {
        T_stmList save_next;
        save_next=now_stm->tail;
        now_stm->tail= nullptr;
        return block_trans(save_next,temp_done);
    }
    else if(now_stm->head->kind==T_stm_::T_LABEL)
    {
        pre_stm->tail= T_StmList(
                T_Jump(T_Name(now_stm->head->u.LABEL),
                       Temp_LabelList(now_stm->head->u.LABEL, nullptr)),
                       nullptr);
        return block_trans(now_stm,temp_done);
    } else
    {
        return cut_stm(now_stm,now_stm->tail,temp_done);
    }
}
C_stmListList block_trans(T_stmList stm,Temp_label temp_done)//负责完成block开头的label检查补充，并调用cut_stm将stm裁剪为block
{
    if(stm== nullptr)
    {return nullptr;}
    else if(stm->head->kind!=T_stm_::T_LABEL)
    {
        Temp_label new_label=Temp_newLabel();
        return (block_trans(T_StmList(T_Label(new_label),stm),temp_done));
    }
    else
    {
        return(C_StmListList(stm,cut_stm(stm,stm->tail,temp_done)));
    }
}
struct C_block C_basicBlocks(T_stmList stmList)
{
    struct C_block my_block{};
    Temp_label temp=Temp_newLabel();
    my_block.labels=temp;//函数出口处理程序所在label
    my_block.stmLists=block_trans(stmList,temp);
    return my_block;

}
static T_stmList getLast(T_stmList list)
{
    T_stmList last = list;
    while (last->tail->tail) last = last->tail;
    return last;
}
static void trace(T_stmList list)
{
    T_stmList last = getLast(list);
    T_stm lab = list->head;
    T_stm s = last->tail->head;
    S_enter(block_env, lab->u.LABEL, nullptr);
    if (s->kind == T_stm_::T_JUMP) {
        auto target = (T_stmList) S_look(block_env, s->u.JUMP.jumps->head);
        if (!s->u.JUMP.jumps->tail && target) {
            last->tail = target;
            trace(target);
        }
        else {last->tail->tail = getNext();}
    }
    else if (s->kind == T_stm_::T_CJUMP) {
        auto trues =  (T_stmList) S_look(block_env, s->u.CJUMP.trues);
        auto falses =  (T_stmList) S_look(block_env, s->u.CJUMP.falses);
        if (falses) {
            last->tail->tail = falses;
            trace(falses);
        }
        else if (trues) {
            last->tail->head = T_Cjump(T_not_op(s->u.CJUMP.op), s->u.CJUMP.left,
                                       s->u.CJUMP.right, s->u.CJUMP.falses,
                    s->u.CJUMP.trues);
            last->tail->tail = trues;
            trace(trues);
        }
        else {
            Temp_label falses = Temp_newLabel();
            last->tail->head = T_Cjump(s->u.CJUMP.op, s->u.CJUMP.left,
                                       s->u.CJUMP.right, s->u.CJUMP.trues, falses);
            last->tail->tail = T_StmList(T_Label(falses), getNext());
        }
    }
    else {printf("error from trace ");assert(0);}
}
static T_stmList getNext()//使所有基本快都被添加到轨迹中
{
    if (!global_block.stmLists)
        return T_StmList(T_Label(global_block.labels), nullptr);
    else {
            T_stmList s = global_block.stmLists->head;
            if (S_look(block_env, s->head->u.LABEL)) {
                trace(s);
                return s;
            }
            else {
                global_block.stmLists = global_block.stmLists->tail;
                return getNext();
            }
    }
}
T_stmList C_traceSchedule(struct C_block b)
{ C_stmListList sList;
    block_env = S_empty();
    global_block = b;

    for (sList=global_block.stmLists; sList; sList=sList->tail) {
        S_enter(block_env, sList->head->head->u.LABEL, sList->head);//
    }
    return getNext();
}