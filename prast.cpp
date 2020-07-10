//
// Created by zcjsh on 2020/7/10.
//

#include "prast.h"

static void pr_var(FILE *out, A_var v, int d);
static void pr_dec(FILE *out, A_dec v, int d);
static void pr_stm(FILE *out, A_stm v, int d);
static void pr_exp(FILE *out, A_exp v, int d);
static void pr_ty(FILE *out, A_ty v, int d);
static void pr_field(FILE *out, A_field v, int d);
static void pr_fieldList(FILE *out, A_fieldList v, int d);
static void pr_expList(FILE *out, A_expList v, int d);
static void pr_comStm(FILE *out, A_comStm v, int d);
static void pr_comStmList(FILE *out, A_comStmList v, int d);
static void pr_case(FILE *out, A_case v, int d);
static void pr_caseList(FILE *out, A_caseList v, int d);
static void pr_initNode(FILE *out, A_initNode v, int d);
static void pr_arraryInitList(FILE *out, A_arrayInitList v, int d);


static void indent(FILE *out,int d){
    int i;
    for(i=0;i<=d;i++) fprintf(out," ");
}

static void pr_var(FILE *out,A_var v,int d)
{
    indent(out,d);
    switch (v->kind) {
        case A_var_::A_simpleVar:
            fprintf(out,"simpleVar(%s)",S_getName(v->u.simple));
            break;
        case A_var_::A_arrayVar:
            fprintf(out,"%s\n","arrayVar(");
            pr_var(out,v->u.arrayVar.id,d+1);
            fprintf(out,"%s\n",",");
            pr_exp(out,v->u.arrayVar.index,d+1);
            fprintf(out,"%s",")");
            break;
        case A_var_::A_structVar:
            fprintf(out,"%s\n","structVar(");
            pr_var(out,v->u.structVar.id,d+1);
            indent(out,d+1);
            fprintf(out,"%s)",S_getName(v->u.structVar.attr));
            break;
        default:
            assert(0);
    }
}

static char str_oper[][12]={
        "or","and","not","lt","le","gt","ge","eq","ne","add","sub","mul","dev","mod"};
static void pr_oper(FILE *out,A_binOp d){
    fprintf(out,"%s",str_oper[d]);
}

static void pr_dec(FILE *out,A_dec v,int d)
{
    indent(out,d);
    switch (v->kind) {
        case A_dec_::A_variableDec:
            fprintf(out,"varibaleDec(%s,\n",S_getName(v->u.var.id));
            if(v->u.var.type){
                indent(out,d+1);
                fprintf(out,"%s,\n",S_getName(v->u.var.type));
            }
            pr_exp(out,v->u.var.init,d+1);
            fprintf(out,",\n");
            indent(out,d+1);
            fprintf(out,"%s",v->u.var.escape?"TRUE)":"FALSE)");
            break;
        case A_dec_::A_arrayDec:
            fprintf(out,"arrayDec(%s,\n",S_getName(v->u.array.id));
            if(v->u.array.base){
                indent(out,d+1);
                fprintf(out,"%s,\n",S_getName(v->u.array.base));
            }
            pr_expList(out,v->u.array.size,d+1);
            fprintf(out,",\n");
            pr_arraryInitList(out,v->u.array.init,d+1);
            fprintf(out,",\n");
            indent(out,d+1);
            fprintf(out,"%s",v->u.array.escape?"TRUE)":"FALSE)");
            break;
        case A_dec_::A_functionDec:
            fprintf(out,"functionDec(%s,\n",S_getName(v->u.function.id));
            pr_fieldList(out,v->u.function.params,d+1);
            fprintf(out,",\n");
            indent(out,d+1);
            fprintf(out,"%s,\n",S_getName(v->u.function.result));
            pr_stm(out,v->u.function.body,d+1);
            fprintf(out,"%s",")");
            break;
        case A_dec_::A_typedef:
            fprintf(out,"typeDec(\n");
            fprintf(out,")");
            break;
        default:
            assert(0);
    }
}

static void pr_stm(FILE *out, A_stm v, int d){
    indent(out,d);
    switch (v->kind) {
        case A_stm_::A_ifStm:
            fprintf(out,"ifStm(\n");
            pr_exp(out,v->u.ifStm.test,d+1);
            fprintf(out,",\n");
            pr_stm(out,v->u.ifStm.body,d+1);
            if(v->u.ifStm.elseBody){
                fprintf(out,",/n");
                pr_stm(out,v->u.ifStm.elseBody,d+1);
            }
            indent(out,d+1);
            fprintf(out,")");
            break;
        case A_stm_::A_whileStm:
            fprintf(out,"whileStm(\n");
            pr_exp(out,v->u.whileStm.test,d+1);
            fprintf(out,",\n");
            pr_stm(out,v->u.whileStm.body,d+1);
            fprintf(out,")\n");
            break;
        case A_stm_::A_blockStm:
            fprintf(out,"blockStm(\n");
            pr_comStmList(out,v->u.blockStm,d+1);
            fprintf(out,")");
            break;
        case A_stm_::A_assignStm:
            fprintf(out,"assignStm(\n");
            pr_var(out,v->u.assignStm.var,d+1);
            fprintf(out,",\n");
            pr_exp(out,v->u.assignStm.exp,d+1);
            fprintf(out,")");
            break;
        case A_stm_::A_switchStm:
            fprintf(out,"switchStm(\n");
            pr_exp(out,v->u.switchStm.exp,d+1);
            fprintf(out,",\n");
            pr_caseList(out,v->u.switchStm.body,d+1);
            fprintf(out,")");
            break;
        case A_stm_::A_returnStm:
            fprintf(out,"returnStm(\n");
            pr_exp(out,v->u.returnStm,d+1);
            fprintf(out,")");
            break;
        case A_stm_::A_breakStm:
            fprintf(out,"breakStm()");
            break;
        case A_stm_::A_continue:
            fprintf(out,"continue()");
            break;
        case A_stm_::A_expStm:
            fprintf(out,"expStm(\n");
            pr_exp(out,v->u.expStm,d+1);
            break;
        default:
            assert(0);
    }
}
static void pr_exp(FILE *out ,A_exp v,int d){
    indent(out,d);
    switch (v->kind) {
        case A_exp_::A_varExp:
            fprintf(out,"varExp(\n");
            pr_var(out,v->u.varExp,d+1);
            fprintf(out,")");
            break;
        case A_exp_::A_intExp:
            fprintf(out,"intExp(%d)",v->u.intExp);
            break;
        case A_exp_::A_charExp:
            fprintf(out,"charExp(%s)",v->u.charExp);
            break;
        case A_exp_::A_callExp:
            fprintf(out,"callExp(%s,\n",S_getName(v->u.callExp.id));
            pr_expList(out,v->u.callExp.args,d+1);
            fprintf(out,")");
            break;
        case A_exp_::A_opExp:
            fprintf(out,"opExp(\n");
            indent(out,d+1);
            pr_oper(out,v->u.opExp.op);
            fprintf(out,",\n");
            pr_exp(out,v->u.opExp.left,d+1);
            fprintf(out,",\n");
            pr_exp(out,v->u.opExp.right,d+1);
            fprintf(out,")");
            break;
        default:
            assert(0);
    }
}

static void pr_ty(FILE *out, A_ty v, int d)
{
    indent(out,d);
    switch (v->kind) {
        case A_ty_::A_aliasTy:
            fprintf(out,"aliasTy(%s)",S_getName(v->u.name));
            break;
        case A_ty_::A_arrayTy:
            fprintf(out,"arrayTy(%s)",S_getName(v->u.name));
            break;
        case A_ty_::A_structTy:
            fprintf(out,"struncTy(\n");
            pr_fieldList(out,v->u.structTy,d+1);
            fprintf(out,")");
            break;
        default:
            assert(0);
    }
}

static void pr_field(FILE *out, A_field v, int d){
    indent(out,d);
    fprintf(out,"field(%s,\n",S_getName(v->id));
    indent(out,d+1);
    fprintf(out,"%s,\n",S_getName(v->type));
    pr_expList(out,v->size,d+1);
    fprintf(out,",\n");
    indent(out,d+1);
    fprintf(out,"%s",v->escape?"TRUE)":"FALSE");
}

static void pr_fieldList(FILE *out, A_fieldList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"fieldList(\n");
        pr_field(out,v->head,d+1);
        fprintf(out,",\n");
        pr_fieldList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"fieldList()");
    }
}

static void pr_expList(FILE *out, A_expList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"expList(\n");
        pr_exp(out,v->head,d+1);
        fprintf(out,",\n");
        pr_expList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"expList()");
    }
}

void pr_decList(FILE *out, A_decList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"decList(\n");
        pr_dec(out,v->head,d+1);
        fprintf(out,",\n");
        pr_decList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"decList()");
    }
}
static void pr_comStm(FILE *out, A_comStm v, int d){
    indent(out,d);
    fprintf(out,"comStm(");
    if(v->const_var_decStm){
        fprintf(out,"\n");
        pr_decList(out,v->const_var_decStm,d+1);
        fprintf(out,",");
    }
    if(v->stmSeq){
        fprintf(out,"\n");
        pr_stm(out,v->stmSeq,d+1);
        fprintf(out,")");
    }else{
        fprintf(out,")");
    }
}

static void pr_comStmList(FILE *out, A_comStmList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"comStmList(\n");
        pr_comStm(out,v->head,d+1);
        fprintf(out,",\n");
        pr_comStmList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"comStmList()");
    }
}

static void pr_case(FILE *out, A_case v, int d){
    indent(out,d);
    fprintf(out,"case(\n");
    fprintf(out,"%d,\n",v->const_value);
    pr_stm(out,v->body,d+1);
    fprintf(out,")");
}

static void pr_caseList(FILE *out, A_caseList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"comStmList(\n");
        pr_case(out,v->head,d+1);
        fprintf(out,",\n");
        pr_caseList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"comStmList()");
    }
}

static void pr_initNode(FILE *out, A_initNode v, int d){
    indent(out,d);
    switch (v->kind) {
        case A_initNode_::A_singleInit:
            fprintf(out,"signleInit(\n");
            pr_exp(out,v->u.single,d+1);
            fprintf(out,")");
            break;
        case A_initNode_::A_nestedInit:
            fprintf(out,"nestedInit(\n");
            pr_arraryInitList(out,v->u.nested,d+1);
            fprintf(out,")");
        default:
            assert(0);
    }
}
static void pr_arraryInitList(FILE *out, A_arrayInitList v, int d){
    indent(out,d);
    if(v){
        fprintf(out,"arrayInitList(\n");
        pr_initNode(out,v->head,d+1);
        fprintf(out,",\n");
        pr_arraryInitList(out,v->tail,d+1);
        fprintf(out,")");
    } else{
        fprintf(out,"arrayInitList()");
    }
}



