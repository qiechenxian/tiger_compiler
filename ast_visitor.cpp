//
// Created by loyx on 2020/8/14.
//

#include "ast_visitor.h"

#define VISIT_MAX_MUN 32
#define CALL_VISIT_FUN(fun, id, element, arg) { \
        if ((visitors_##fun[id]) != nullptr){ \
            if((visitors_##fun[id])(element, arg)){ \
                walkOn_##fun(element, id, arg); \
            } \
        } \
    }

static void walkOn_stm(A_stm s, int visitor_id, void* arg);
static void walkOn_exp(A_exp e, int visitor_id, void* arg);
static void walkOn_dec(A_dec d, int visitor_id, void* arg);
static void walkOn_var(A_var v, int visitor_id, void* arg);

static visit_fun_stm visitors_stm[VISIT_MAX_MUN];
static visit_fun_exp visitors_exp[VISIT_MAX_MUN];
static visit_fun_dec visitors_dec[VISIT_MAX_MUN];
static visit_fun_var visitors_var[VISIT_MAX_MUN];
static int visitor_num = 0;

bool default_visit_fun_stm(A_stm s, void* arg){return true;}
bool default_visit_fun_exp(A_exp e, void* arg){return true;}
bool default_visit_fun_dec(A_dec d, void* arg){return true;}
bool default_visit_fun_var(A_var v, void* arg){return true;}

int ASTV_registerDefaultVisitors(){
    return ASTV_registerVisitors(
            default_visit_fun_dec,
            default_visit_fun_exp,
            default_visit_fun_stm,
            default_visit_fun_var
            );
}

int ASTV_registerDecVisitor(visit_fun_dec fun){
    int visit_id = ASTV_registerDefaultVisitors();
    visitors_dec[visit_id] = fun;
    return visit_id;
}
int ASTV_registerExpVisitor(visit_fun_exp fun){
    int visit_id = ASTV_registerDefaultVisitors();
    visitors_exp[visit_id] = fun;
    return visit_id;
}
int ASTV_registerStmVisitor(visit_fun_stm fun){
    int visit_id = ASTV_registerDefaultVisitors();
    visitors_stm[visit_id] = fun;
    return visit_id;
}
int ASTV_registerVarVisitor(visit_fun_var fun){
    int visit_id = ASTV_registerDefaultVisitors();
    visitors_var[visit_id] = fun;
    return visit_id;
}

int ASTV_registerVisitors(visit_fun_dec f_dec, visit_fun_exp f_exp,
                    visit_fun_stm f_stm, visit_fun_var f_var){
    int visitor_id = visitor_num ++;
    assert(visitor_id < VISIT_MAX_MUN);
    if (f_dec != nullptr)
        visitors_dec[visitor_id] = f_dec;
    if (f_exp != nullptr)
        visitors_exp[visitor_id] = f_exp;
    if (f_stm != nullptr)
        visitors_stm[visitor_id] = f_stm;
    if (f_var != nullptr)
        visitors_var[visitor_id] = f_var;
    return visitor_id;
}

void ASTV_iterDec(A_dec el, int visitor_id, void* arg){
    walkOn_dec(el, visitor_id, arg);
}
void ASTV_iterExp(A_exp el, int visitor_id, void* arg){
    walkOn_exp(el, visitor_id, arg);
}
void ASTV_iterStm(A_stm el, int visitor_id, void* arg){
    walkOn_stm(el, visitor_id, arg);
}
void ASTV_iterVar(A_var el, int visitor_id, void* arg){
    walkOn_var(el, visitor_id, arg);
}

static void walkOn_stm(A_stm s, int visitor_id, void* arg){
    switch (s->kind) {
        case A_stm_::A_ifStm:{
            CALL_VISIT_FUN(exp, visitor_id, s->u.ifStm.test, arg);
            CALL_VISIT_FUN(stm, visitor_id, s->u.ifStm.body, arg);
            if (s->u.ifStm.elseBody != nullptr)
                CALL_VISIT_FUN(stm, visitor_id, s->u.ifStm.elseBody, arg);
            break;
        }
        case A_stm_::A_whileStm:{
            CALL_VISIT_FUN(exp, visitor_id, s->u.whileStm.test, arg);
            CALL_VISIT_FUN(stm, visitor_id, s->u.whileStm.body, arg);
            break;
        }
        case A_stm_::A_blockStm:{
            for (A_comStmList iter = s->u.blockStm; iter; iter = iter->tail){
                if (iter->head->const_var_decStm){
                    A_decList dec_iter = iter->head->const_var_decStm;
                    for (; dec_iter; dec_iter = dec_iter->tail){
                        CALL_VISIT_FUN(dec, visitor_id, dec_iter->head, arg);
                    }

                } else if (iter->head->stmSeq){
                    CALL_VISIT_FUN(stm, visitor_id, iter->head->stmSeq, arg);
                }
            }
            break;
        }
        case A_stm_::A_assignStm:{
            CALL_VISIT_FUN(var, visitor_id, s->u.assignStm.var, arg);
            CALL_VISIT_FUN(exp, visitor_id, s->u.assignStm.exp, arg);
            break;
        }
        case A_stm_::A_switchStm:{
            assert(0);
            break;
        }
        case A_stm_::A_returnStm:{
            if (s->u.returnStm){
                CALL_VISIT_FUN(exp, visitor_id, s->u.returnStm, arg);
            }
            break;
        }
        case A_stm_::A_breakStm:
        case A_stm_::A_continue:{
            break;
        }
        case A_stm_::A_expStm:{
            CALL_VISIT_FUN(exp, visitor_id, s->u.expStm, arg);
            break;
        }
    }
}

static void walkOn_exp(A_exp e, int visitor_id, void* arg){
    switch (e->kind) {

        case A_exp_::A_varExp:
            CALL_VISIT_FUN(var, visitor_id, e->u.varExp, arg);
            break;
        case A_exp_::A_intExp:
        case A_exp_::A_charExp:
            break;
        case A_exp_::A_callExp:
            for (A_expList iter = e->u.callExp.args; iter; iter = iter->tail){
                CALL_VISIT_FUN(exp, visitor_id, iter->head, arg);
            }
            break;
        case A_exp_::A_opExp:
            if (e->u.opExp.left)
                CALL_VISIT_FUN(exp, visitor_id, e->u.opExp.left, arg);
            if (e->u.opExp.right)
                CALL_VISIT_FUN(exp, visitor_id, e->u.opExp.right, arg);
            break;
        case A_exp_::A_stringExp:
            break;
    }

}
static void walkOn_dec(A_dec d, int visitor_id, void* arg){
    switch (d->kind) {

        case A_dec_::A_variableDec:
            CALL_VISIT_FUN(exp, visitor_id, d->u.var.init, arg);
            break;
        case A_dec_::A_arrayDec:
            assert(0);
            //todo initList
            break;
        case A_dec_::A_functionDec:
            assert(0);
            break;
        case A_dec_::A_typedef:
            assert(0);
            break;
    }
}
static void walkOn_var(A_var v, int visitor_id, void* arg){
    switch (v->kind) {

        case A_var_::A_simpleVar:
            break;
        case A_var_::A_arrayVar:
            CALL_VISIT_FUN(var, visitor_id, v->u.arrayVar.id, arg);
            CALL_VISIT_FUN(exp, visitor_id, v->u.arrayVar.index, arg);
            break;
        case A_var_::A_structVar:
            break;
    }
}
