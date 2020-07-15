//
// Created by Administrator on 11/7/2020.
//
#include "cannon.h"
typedef struct expRefList_* expRefList;
struct expRefList_ {T_exp *head;expRefList tail;};
struct stmExp{T_stm s;T_exp e;};
static T_stm recoder(expRefList rlist);
static T_stm do_stm(T_stm stm);
static struct stmExp do_exp(T_exp exp);
static expRefList get_call_reflist(T_exp callExp);
static T_stmList linear(T_stm stm,T_stmList right);
struct C_block global_block;
S_table block_env;
static T_stmList getLast(T_stmList list);
static void trace(T_stmList list);
static T_stmList getNext();
T_stmList C_traceSchedule(struct C_block b);
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
    struct stmExp new_stmExp;
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
    return linear(do_stm(stm),NULL);
}
static T_stmList linear(T_stm stm,T_stmList right)//seq树节点线性化
{
    if(stm->kind==T_stm_::T_SEQ)
    {
        return linear(stm->u.SEQ.left,linear(stm->u.SEQ.right,right));
    } else
    {return T_StmList(stm,right);}
}
static expRefList get_call_reflist(T_exp callExp) {
    expRefList res = ExpRefList(&callExp->u.CALL.fun, NULL);
    expRefList p = res;
    T_expList p_args = callExp->u.CALL.args;
    while (p_args) {
        p->tail = ExpRefList(&p_args->head, NULL);
        p_args = p_args->tail;
        p = p->tail;
    }
    return res;
}
static T_stm do_stm(T_stm stm) {//对stm的子节点调用recoder
    switch (stm->kind) {
        case T_stm_::T_SEQ:
            return seq(do_stm(stm->u.SEQ.left), do_stm(stm->u.SEQ.right));
        case T_stm_::T_JUMP:
            return seq(recoder(ExpRefList(&stm->u.JUMP.exp, NULL)),stm);
        case T_stm_::T_CJUMP:
            return seq(
                    recoder(ExpRefList(&stm->u.CJUMP.left,ExpRefList(&stm->u.CJUMP.right, NULL))),stm);
        case T_stm_::T_EXP:
            if (stm->u.EXP->kind == T_exp_::T_CALL)//call的处理
                return seq(recoder(get_call_reflist(stm->u.EXP)),stm);
            else
                return seq(recoder(ExpRefList(&stm->u.EXP, NULL)),stm);
        case T_stm_::T_MOVE:
            if (stm->u.MOVE.dst->kind == T_exp_::T_TEMP) {
                if (stm->u.MOVE.src->kind == T_exp_::T_CALL)
                    return seq(recoder(get_call_reflist(stm->u.MOVE.src)),stm);
                else
                    return seq(recoder(ExpRefList(&stm->u.MOVE.src, NULL)),stm);
            } else if (stm->u.MOVE.dst->kind == T_exp_::T_MEM) {
                return seq(recoder(ExpRefList(&stm->u.MOVE.dst->u.MEM,ExpRefList(&stm->u.MOVE.src, NULL))),stm);
            } else if (stm->u.MOVE.dst->kind == T_exp_::T_ESEQ) {
                T_stm s = stm->u.MOVE.dst->u.ESEQ.stm;
                stm->u.MOVE.dst = s->u.MOVE.dst->u.ESEQ.exp;
                return do_stm(T_Seq(s, stm)); // todo:
            }
        default:
            return stm;
    }
}
static struct stmExp do_exp(T_exp exp){//对exp的子节点调用recoder
    switch (exp->kind) {
        case T_exp_::T_BINOP:
            return StmExp(recoder(ExpRefList(&exp->u.BINOP.left,ExpRefList(&exp->u.BINOP.right, nullptr))),exp);
        case T_exp_::T_MEM:
            return StmExp(recoder(ExpRefList(&exp->u.MEM, nullptr)),exp);
        case T_exp_::T_ESEQ:{struct  stmExp x=do_exp(exp->u.ESEQ.exp);
        return StmExp(seq(do_stm(exp->u.ESEQ.stm),x.s),x.e);
        }
        case T_exp_::T_CALL:
            return StmExp(recoder(get_call_reflist(exp)),exp);
        default:
            return StmExp(recoder(nullptr),exp);
    }
}
static T_stm recoder(expRefList refList) {//完成对ESEQ的消除，将ESEQ的stm提前，并将ESEQ用ESEQ的exp替换或者用保存有exp的temp替换
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

    if (commute(others, head.e)) {//如果others的stm可以放在head的exp前，顺序修改对head.exp无影响
        *refList->head = head.e;
        return seq(head.s, others);
    } else//顺序修改对head.exp有影响，将head.exp先保存到寄存器中
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
    if(now_stm == nullptr)//函数结束跳转至出口处理
    {
        T_stmList done_blcok= T_StmList(T_Jump(T_Name(temp_done),Temp_LabelList(temp_done, nullptr)), nullptr);
        pre_stm->tail=done_blcok;
        return nullptr;
    }
    else if(now_stm->head->kind==T_stm_::T_CJUMP||now_stm->head->kind==T_stm_::T_JUMP)//基本块结束，切断后，开始新的基本块
    {
        T_stmList save_next;
        save_next=now_stm->tail;
        now_stm->tail= nullptr;
        return block_trans(save_next,temp_done);
    }
    else if(now_stm->head->kind==T_stm_::T_LABEL)//基本块结束，添加T_jumo，切断，开始新的基本块
    {
        pre_stm->tail= T_StmList(T_Jump(T_Name(now_stm->head->u.LABEL),Temp_LabelList(now_stm->head->u.LABEL, nullptr)), nullptr);
        return block_trans(now_stm,temp_done);
    } else
    {
        return cut_stm(now_stm,now_stm->tail,temp_done);//空操作，往后继续
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
    struct C_block my_block;
    Temp_label temp=Temp_newLabel();
    my_block.labels=temp;//函数出口处理程序所在label，实现在后面，尚未完成
    my_block.stmLists=block_trans(stmList,temp);
    return my_block;

}
static T_stmList getLast(T_stmList list)//得到基本块倒数第二个stm（T_jump前或T_Cjump前）
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
    S_enter(block_env, lab->u.LABEL, NULL);
    if (s->kind == T_stm_::T_JUMP) {//将jump的目标块放置到jump后
        T_stmList target = (T_stmList) S_look(block_env, s->u.JUMP.jumps->head);
        if (!s->u.JUMP.jumps->tail && target) {
            last->tail = target;
            trace(target);
        }
        else {last->tail->tail = getNext();}
    }
    else if (s->kind == T_stm_::T_CJUMP) {//将cjump的false目标块放置到cjump后
        T_stmList trues =  (T_stmList) S_look(block_env, s->u.CJUMP.trues);
        T_stmList falses =  (T_stmList) S_look(block_env, s->u.CJUMP.falses);
        if (falses) {//有false情况
            last->tail->tail = falses;
            trace(falses);
        }
        else if (trues) {//只有true情况，反转条件，true与false label反转
            last->tail->head = T_Cjump(T_not_op(s->u.CJUMP.op), s->u.CJUMP.left,
                                       s->u.CJUMP.right, s->u.CJUMP.falses,
                    s->u.CJUMP.trues);
            last->tail->tail = trues;
            trace(trues);
        }
        else {//都没有
            Temp_label falses = Temp_newLabel();
            last->tail->head = T_Cjump(s->u.CJUMP.op, s->u.CJUMP.left,
                                       s->u.CJUMP.right, s->u.CJUMP.trues, falses);
            last->tail->tail = T_StmList(T_Label(falses), getNext());
        }
    }
    else {printf("error from trace ");assert(0);}
}
static T_stmList getNext()
{
    if (!global_block.stmLists)//基本块为空
        return T_StmList(T_Label(global_block.labels), NULL);
    else {
            T_stmList s = global_block.stmLists->head;
            if (S_look(block_env, s->head->u.LABEL)) {//stm在block中
                trace(s);//轨迹处理
                return s;
            }
            else {
                global_block.stmLists = global_block.stmLists->tail;
                return getNext();
            }
    }
}
T_stmList C_traceSchedule(struct C_block b)//轨迹
{ C_stmListList sList;
    block_env = S_empty();
    global_block = b;

    for (sList=global_block.stmLists; sList; sList=sList->tail) {
        S_enter(block_env, sList->head->head->u.LABEL, sList->head);//标记所有块中stm
    }
    return getNext();//返回处理好的轨迹
}